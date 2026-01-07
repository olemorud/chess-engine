/* this code is complete garbage but it's mostly for testing anyways */

const MoveResult = Object.freeze({
    ILLEGAL:  -1,
    NORMAL:    0,
    CHECK:     1,
    REPEATS:   2,
    STALEMATE: 3,
    CHECKMATE: 4,
});

const _moveResultName = [
    "Normal",
    "Check",
    "Repeats",
    "Stalemate",
    "Checkmate",
];

const moveResultName = (mr) => {
    if (mr === MoveResult.ILLEGAL) return "Illegal";
    return _moveResultName[mr] ?? `Unknown(${mr})`;
};

const isTerminal = (mr) => (mr === MoveResult.STALEMATE || mr === MoveResult.CHECKMATE);

const Player = Object.freeze({
    White: 0,
    Black: 1,
    None:  2,
});

const Piece = Object.freeze({
	Empty:  0,
    Pawn:   1,
    Knight: 2,
    Bishop: 3,
    Rook:   4,
    Queen:  5,
    King:   6,
});

const _pieceUnicode = [
    [' ', '♙', '♘', '♗', '♖', '♕', '♔',], // white
    [' ', '♟', '♞', '♝', '♜', '♛', '♚',], // black
    [' ', ' ', ' ', ' ', ' ', ' ', ' ',], // none
];

const indexDeserialize = (n) => ({
    rank: Math.floor(n / 8),
    file: (n % 8),
    fileChar() { return ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'][this.file]; },
    rankChar() { return ['1', '2', '3', '4', '5', '6', '7', '8'][this.rank]; },
});

const serializedIndexFromCoord = (rank, file) => (8 * rank + file);
const indexSerialize = (index) => serializedIndexFromCoord(index.rank, index.file);

const moveDeserialize = (n) => ({
    appeal: Number((n >> 24n) & 0xFFn),
    attr:   Number((n >> 16n) & 0xFFn),
    from:   indexDeserialize(Number((n >>  8n) & 0xFFn)),
    to:     indexDeserialize(Number((n >>  0n) & 0xFFn)),
});

const moveSerialize = (move) => (
    ((indexSerialize(move.from) & 0xFF) << 8) | (indexSerialize(move.to) & 0xFF)
);

const squareDeserialize = (n) => {
	if (n === -1) {
		return {
			player: Player.None,
			piece: Piece.Empty,
			unicode: ' ',
		};
	}

	const player  = (n >> 8) & 0xFF;
	const piece   = n & 0xFF;
	const unicode = _pieceUnicode[player][piece];

	return {
		player: player,
		piece:  piece,
		unicode: unicode,
	};
};

const WasmBoard = async () => {
    const worker = new Worker("./chess-worker.js?rand=" + crypto.randomUUID(), { type: "module" });

    await new Promise((resolve, reject) => {
        worker.addEventListener("message", (e) => {
            if (e.data?.type === "ready") resolve();
        }, { once: true });
        worker.addEventListener("error", reject, { once: true });
    });

    const wasmCall = (method, args = []) => new Promise((resolve, reject) => {
        const id = crypto.randomUUID();

        const onMessage = (e) => {
            if (e.data?.id !== id) return;
            cleanup();
            if (e.data.ok) resolve(e.data.value);
            else reject(new Error(e.data.error));
        };
        const onError = (err) => { cleanup(); reject(err); };
        const onMessageError = () => { cleanup(); reject(new Error("Worker messageerror")); };

        const cleanup = () => {
            worker.removeEventListener("message", onMessage);
            worker.removeEventListener("error", onError);
            worker.removeEventListener("messageerror", onMessageError);
        };

        worker.addEventListener("message", onMessage);
        worker.addEventListener("error", onError);
        worker.addEventListener("messageerror", onMessageError);

        worker.postMessage({ id, method, args });
    });

    await wasmCall("wb_init", []);

    return {
        state: MoveResult.NORMAL,

        search: async function(depth, timeout) {
            return wasmCall("wb_search", [depth]).then(moveDeserialize);
        },

        move: async function(move) {
            const m = (typeof move === "number") ? move : moveSerialize(move);
            const mr = await wasmCall("wb_move", [m]);

            if (mr === MoveResult.NORMAL || mr === MoveResult.CHECK || mr === MoveResult.REPEATS ||
                mr === MoveResult.STALEMATE || mr === MoveResult.CHECKMATE) {
                this.state = mr;
                return mr;
            }
            return MoveResult.ILLEGAL;
        },

        at: async function(index) {
            const i = (typeof index === "number") ? index : indexSerialize(index);
            return wasmCall("wb_board_at", [i])
					.then(squareDeserialize);
        },

        legalMoves: async function() {
            const n = await wasmCall("wb_all_moves_len", []);
            const out = new Uint16Array(n);
            for (let i = 0; i < n; ++i) {
                out[i] = await wasmCall("wb_all_moves_get", [i]);
            }
            return out;
        },
    };
};

const run = async () => {
    const board = await WasmBoard();

    const boardElem = document.getElementById("board");
    const resultEl = document.getElementById("result");
    const setStatus = (s) => { resultEl.textContent = s; };

    const squares = new Array(64);

    const idxToAlg = (idx) => {
        const i = indexDeserialize(idx);
        return `${i.fileChar()}${i.rankChar()}`;
    };

    const draw = async () => {
        for (let rank = 7; rank >= 0; --rank) {
            for (let file = 0; file < 8; ++file) {
                const idx = 8 * rank + file;
                const sq = await board.at(idx);
				console.log(sq);
				console.log(sq.unicode);
                squares[idx].textContent = sq.unicode;
            }
        }
    };

    const createBoardUI = () => {
        boardElem.replaceChildren();
        for (let rank = 7; rank >= 0; --rank) {
            for (let file = 0; file < 8; ++file) {
                const idx = 8 * rank + file;
                const el = document.createElement("div");
                el.className = "square " + (((file + rank) % 2) ? "dark" : "light");
                el.dataset.idx = String(idx);
                boardElem.appendChild(el);
                squares[idx] = el;
            }
        }
    };

    createBoardUI();
    await draw();

    let selected = null;     // 0..63 or null
    let inputEnabled = true;

    const clearSelection = () => {
        if (selected !== null) squares[selected].classList.remove("blue");
        selected = null;
    };

    const legalHasFrom = (moves, fromIdx) => {
        for (const mv of moves) {
            if (((mv >> 8) & 0xFF) === fromIdx) return true;
        }
        return false;
    };

    const legalHasMove = (moves, fromIdx, toIdx) => {
        const key = ((fromIdx & 0xFF) << 8) | (toIdx & 0xFF);
        for (const mv of moves) {
            if (mv === key) return true;
        }
        return false;
    };

    const engineReply = async (depth) => {
        console.log("searching");
        setStatus("Black thinking...");
        const m = await board.search(depth, 2000);
        console.log("found move", m, );
        let sooner = Date.now();
        const mr = await board.move(m);
        await draw();
        let later = Date.now();
        let seconds = later - sooner;
        setStatus(`Black played ${idxToAlg(indexSerialize(m.from))}-${idxToAlg(indexSerialize(m.to))} after ${seconds} seconds\nWhite to move...`);
        return mr;
    };

    const onSquareClick = async (idx) => {
        if (!inputEnabled) return;
        if (isTerminal(board.state)) return;

        const legalMoves = await board.legalMoves();

        if (selected === null) {
            const sq = await board.at(idx);
            if (sq.player !== Player.White) return;

            if (!legalHasFrom(legalMoves, idx)) {
                clearSelection();
                setStatus(`No legal moves from ${idxToAlg(idx)}.`);
                return;
            }

            selected = idx;
            squares[selected].classList.add("blue");
            setStatus(`Selected ${idxToAlg(idx)}.`);
            return;
        }

        if (idx === selected) {
            clearSelection();
            setStatus("");
            return;
        }

        const clickedSq = await board.at(idx);

        // user clicks another white piece: either switch selection (if it has moves) or deselect
        if (clickedSq.player === Player.White) {
            clearSelection();

            if (!legalHasFrom(legalMoves, idx)) {
                setStatus(`No legal moves from ${idxToAlg(idx)}.`);
                return;
            }

            selected = idx;
            squares[selected].classList.add("blue");
            setStatus(`Selected ${idxToAlg(idx)}.`);
            return;
        }

        const from = selected;
        const to = idx;

        if (!legalHasMove(legalMoves, from, to)) {
            clearSelection();
            setStatus(`Illegal: ${idxToAlg(from)}-${idxToAlg(to)}.`);
            return;
        }

        clearSelection();
        inputEnabled = false;

        const mrWhite = await board.move({ from: indexDeserialize(from), to: indexDeserialize(to) });
        await draw();
        setStatus(`White played ${idxToAlg(from)}-${idxToAlg(to)}. ${moveResultName(mrWhite)}`);

        if (isTerminal(mrWhite)) {
            inputEnabled = false;
            return;
        }

        const mrBlack = await engineReply(8);
        if (isTerminal(mrBlack)) {
            inputEnabled = false;
            return;
        }

        inputEnabled = true;
    };

    for (let i = 0; i < 64; ++i) {
        squares[i].addEventListener("click", () => { onSquareClick(i).catch(console.error); });
    }

    setStatus("White to move.");
};

run().catch((err) => console.error(err));

