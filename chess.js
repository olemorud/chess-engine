
const MoveResult = Object.freeze({
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

const Player = Object.freeze({
    White: 0,
    Black: 1,
    None:  2,
});

const _playerName = [
    "White",
    "Black",
    "None",
];

const Piece = Object.freeze({
    Pawn:   0,
    King:   1,
    Queen:  2,
    Bishop: 3,
    Rook:   4,
    Knight: 5,
    Empty:  6,
});

const _pieceName = [
    "Pawn",
    "King",
    "Queen",
    "Bishop",
    "Rook",
    "Knight",
    "Empty",
];

const _pieceChar = [
    'P',
    'K',
    'Q',
    'B',
    'R',
    'N',
    ' ',
];

const _pieceUnicode = [
    ['♙', '♔', '♕', '♗', '♖', '♘',], // white
    ['♟', '♚', '♛', '♝', '♜', '♞',], // black
    ['' , '',  '',  '',  '',  '', ],  // none
];

const indexDeserialize = (n) => ({
    rank: Math.floor(n / 8), // every day we stray further from God
    file: (n % 8),
    fileChar() { return ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'][this.file]; },
    rankChar() { return ['1', '2', '3', '4', '5', '6', '7', '8'][this.rank]; },
});


const serializedIndexFromCoord = (rank, file) => (8*rank + file);

const indexSerialize = (index) => (serializedIndexFromCoord(index.rank, index.file));

const moveDeserialize = (n) => ({
    appeal: Number((n >> 24n) & 0xFFn),
    attr:   Number((n >> 16n) & 0xFFn),
    from:   indexDeserialize(Number((n >>  8n) & 0xFFn)),
    to:     indexDeserialize(Number((n >>  0n) & 0xFFn)),
});

const moveSerialize = (move) => (
    (indexSerialize(move.from) & 0xFF) << 8) | ((indexSerialize(move.to) & 0xFF));

const squareDeserialize = (n) => ({
    player: n == -1 ? Player.None : (n >> 8) & 0xFF,
    piece:  n == -1 ? Piece.Empty : (n & 0xFF),
    playerName() {
        if (this.player === Player.None) {
            return "Empty";
        } else {
            return _playerName[this.player];
        }
    },
    pieceName() {
        if (this.player === Player.None) {
            return "";
        } else {
            return this.playerName() +  _pieceName[this.piece];
        }
    },
    pieceUnicode() {
        return _pieceUnicode[this.player][this.piece];
    }
});

const WasmBoard = async (url) => {
    /*
    const res = await fetch(url);
    if (!res.ok) {
        throw new Error(`Failed to fetch ${url}: ${res.status} ${res.statusText}`);
    }
    const bytes = await res.arrayBuffer();
    const instantiated = await WebAssembly.instantiate(bytes, {});
    const instance = instantiated.instance ?? instantiated;
    const exports = instance.exports;

    const wb_init     = exports.wb_init;
    const wb_move     = exports.wb_move;
    const wb_search   = exports.wb_search;
    const wb_board_at = exports.wb_board_at;
    */
    const worker = new Worker("./chess-worker.js", { type: "module" });

    await new Promise((resolve, reject) => {
        worker.addEventListener("message", (e) => {
            if (e.data?.type === "ready") {
                resolve();
            }
        }, { once: true });
        worker.addEventListener("error", reject, { once: true });
    });

    const wasmCall = (method, args = []) => new Promise((resolve, reject) => {
        const id = crypto.randomUUID();
        const onMessage = (e) => {
            if (e.data?.id !== id) {
                return;
            }
            cleanup();
            if (e.data.ok) {
                resolve(e.data.value);
            } else {
                reject(new Error(e.data.error));
            }
        };

        const onError = (err) => {
            cleanup();
            reject(err);
        };

        const onMessageError = (e) => {
            cleanup();
            reject(new Error("Worker message deserialization failed (messageerror)."));
        };

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
        state: MoveResult.Normal,

        ongoing: function() /* nil -> bool */ {
            return (this.state != MoveResult.STALEMATE 
                 && this.state != MoveResult.CHECKMATE);
        },

        search: async function(depth) /* Int -> Move */ {
            return wasmCall("wb_search", [depth])
                        .then(moveDeserialize);
        },

        move: async function (move) /* (Move | SerializedMove) -> MoveResult */ {
            const m = (typeof move === "number") ? move : moveSerialize(move);
            return wasmCall("wb_move", [m]);
        },

        at: async function (index) /* (Index | SerializedIndex) -> Square */ {
            const i = (typeof index === "number") ? index : indexSerialize(index);
            return wasmCall("wb_board_at", [i])
                    .then(squareDeserialize);
        },
    };
};


const run = async () => {
    const board = await WasmBoard("./chess-worker.js");

    const nextFrame = () => new Promise(requestAnimationFrame);

    const createBoardUI = async (board) => {
        const boardElem = document.getElementById("board");
        boardElem.replaceChildren();
        const filesChars = ["a","b","c","d","e","f","g","h"];
        const ranksChars = ["8","7","6","5","4","3","2","1"];

        const squares = new Array(64);

        for (let rank = 7; rank >= 0; --rank) {
            for (let file = 0; file < 8; ++file) {
                const el = document.createElement("div");
                el.className =
                    "square " + ((file + rank) % 2 ? "dark" : "light");
                el.dataset.file = filesChars[file];
                el.dataset.rank = ranksChars[rank];

                const sq = await board.at(serializedIndexFromCoord(rank, file));
                el.textContent = sq.pieceUnicode();

                boardElem.appendChild(el);
                squares[8*rank+file] = el;
            }
        }

        return squares;
    };

    const draw = async (board, squareElems) => {
        for (let rank = 7; rank >= 0; --rank) {
            for (let file = 0; file < 8; ++file) {
                const sq = await board.at(serializedIndexFromCoord(rank, file));
                squareElems[8*rank+file].textContent = sq.pieceUnicode();
            }
        }
    };

    const sqElems = await createBoardUI(board);
    for (let i = 0; i < 200; ++i) {
        await draw(board, sqElems);
        await nextFrame();

        const move = await board.search(7);

        const fromSq = await board.at(move.from);
        const toSq   = await board.at(move.to);

        const mr = board.move(move);
        if (mr == MoveResult.STALEMATE || mr == MoveResult.CHECKMATE) {
            const resultEl = document.getElementById("result");
            resultEl.textContent = _moveResultName[board.state];
            draw(board, sqElems);
            await nextFrame();
            break;
        }
    }
}

run().catch((err) => {
    console.error(err);
});


