
const WasmHost = (() => {
    async function load(url, imports = {}) {
        const res = await fetch(url);
        if (!res.ok) {
            throw new Error(`Failed to fetch ${url}: ${res.status} ${res.statusText}`);
        }
        const bytes = await res.arrayBuffer();
        const result = await WebAssembly.instantiate(bytes, imports);
        const instance = result.instance ?? result;
        return { instance, exports: instance.exports };
    }

    function getFn(exportsObj, name) {
        const fn = exportsObj[name];
        if (typeof fn !== "function") {
            throw new Error(`Export "${name}" is not a function`);
        }
        return fn;
    }

    return { load, getFn };
})();

const MoveResult = Object.freeze({
	Normal:    0,
    Check:     1,
	Repeats:   2,
    Stalemate: 3,
    Checkmate: 4,
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


const indexFromCoord = (rank, file) => (8*rank + file);

const indexSerialize = (index) => (indexFromCoord(index.rank, index.file));

const moveDeserialize = (n) => ({
	appeal: Number((n >> 24n) & 0xFFn),
	attr:   Number((n >> 16n) & 0xFFn),
	from:   indexDeserialize(Number((n >>  8n) & 0xFFn)),
	to:     indexDeserialize(Number((n >>  0n) & 0xFFn)),
});

const moveSerialize = (move) => ((move.from & 0xFF) << 8) | ((move.to & 0xFF));

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



const run = async () => {
    const { exports } = await WasmHost.load("./chess.wasm", {});

    const wb_init     = WasmHost.getFn(exports, "wb_init");
    const wb_move     = WasmHost.getFn(exports, "wb_move");
    const wb_search   = WasmHost.getFn(exports, "wb_search");
    const wb_board_at = WasmHost.getFn(exports, "wb_board_at");

    wb_init();

	const drawBoard = () => {
		const board = document.getElementById("board");
		board.replaceChildren();
		const filesChars = ["a","b","c","d","e","f","g","h"];
		const ranksChars = ["8","7","6","5","4","3","2","1"];

		for (let rank = 7; rank >= 0; --rank) {
			for (let file = 0; file < 8; ++file) {
				const square = document.createElement("div");
				square.className =
					"square " + ((file + rank) % 2 ? "dark" : "light");
				square.dataset.file = filesChars[file];
				square.dataset.rank = ranksChars[rank];

				const x = wb_board_at(indexFromCoord(rank, file));
				const sq = squareDeserialize(x);
				square.textContent = sq.pieceUnicode();

				board.appendChild(square);
			}
		}
		setTimeout(() => {
		  // next task, browser had a chance to repaint
		}, 0.01);
	}
	drawBoard();

	const nextFrame = () => new Promise(requestAnimationFrame);

	for (let i = 0; i < 200; ++i) {
		drawBoard();

		await nextFrame();

		const m = wb_search(7);
		const move = moveDeserialize(m);

		console.log(move.from);
		console.log(move.to);

		const fromSqEnc = wb_board_at(indexSerialize(move.from));
		const toSqEnc = wb_board_at(indexSerialize(move.to));

		console.log(fromSqEnc);
		console.log(toSqEnc);

		const fromSq = squareDeserialize(fromSqEnc);
		const toSq   = squareDeserialize(toSqEnc);

		console.log(fromSq)
		console.log(toSq)

		console.log("from-player:", fromSq.playerName());
		console.log("from-piece:",  fromSq.pieceName());

		console.log("to-player:", toSq.playerName());
		console.log("to-piece:",  toSq.pieceName());

		console.log(m);
		const f = move.from;
		const t = move.to;
		console.log("from:", move.from.fileChar(), move.from.rankChar(),
					"to:",   move.to.fileChar(), move.to.rankChar());

		const mr = wb_move(Number(m));
		if (mr == MoveResult.Stalemate || mr == MoveResult.Checkmate) {
			console.log(_moveResultName[mr]);
			break;
		}
	}
}

run().catch((err) => {
    console.error(err);
});


