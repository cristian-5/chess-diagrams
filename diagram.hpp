
#ifndef DIAGRAM_HPP
#define DIAGRAM_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>

using namespace std;

const bool WHITE = true;
const bool BLACK = false;
using perspective = bool;
using player = bool;
using pixel = uint32_t;

enum files { A, B, C, D, E, F, G, H };

enum pixel_type: unsigned { rgb = 3, rgba = 4 };
template <pixel_type P>
class image {

	private:

	uint8_t * data;
	size_t size, w, h;

	public:

	image(string name, size_t h, size_t w) {
		ifstream file(name.c_str(), ios::binary);
		if (!file.good()) return;
		file.seekg(0, ios::end);
		this -> size = file.tellg();
		file.seekg(0, ios::beg);
		this -> data = new uint8_t[this -> size];
		file.read((char *) this -> data, this -> size);
		file.close();
		this -> w = w; this -> h = h;
	}

	image(string name, size_t s) {
		ifstream file(name.c_str(), ios::binary);
		if (!file.good()) return;
		file.seekg(0, ios::end);
		this -> size = file.tellg();
		file.seekg(0, ios::beg);
		this -> data = new uint8_t[size];
		file.read((char *) this -> data, size);
		file.close();
		this -> w = s; this -> h = s;
	}

	image(uint8_t * data, size_t w, size_t h) {
		this -> data = data;
		this -> size = h * w * P;
		this -> w = w; this -> h = h;
	}

	image(uint8_t * data, size_t s) {
		this -> data = data;
		this -> size = s * s * P;
		this -> w = s; this -> h = s;
	}

	pixel get(size_t x, size_t y) {
		const size_t i = (y * w + x) * P;
		return data[i] << 24 | data[i + 1] << 16 | data[i + 2] << 8 |
			(P == rgba ? data[i + 3] : 0xFF);
	}

	void set(pixel p, size_t x, size_t y) {
		const size_t i = (y * w + x) * P;
		data[i] = p >> 24 & 0xFF;
		data[i + 1] = p >> 16 & 0xFF;
		data[i + 2] = p >> 8 & 0xFF;
		if (P == rgba) data[i + 3] = p & 0xFF;
	}

	void blend(pixel p, size_t x, size_t y) {
		const size_t i = (y * w + x) * P;
		const uint8_t a = p & 0xFF;
		data[i] = (data[i] * (0xFF - a) + (p >> 24 & 0xFF) * a) >> 8;
		data[i + 1] = (data[i + 1] * (0xFF - a) + (p >> 16 & 0xFF) * a) >> 8;
		data[i + 2] = (data[i + 2] * (0xFF - a) + (p >> 8 & 0xFF) * a) >> 8;
		if (P == rgba) data[i + 3] = (data[i + 3] * (0xFF - a) + a) >> 8;
	}

	template <pixel_type T>
	void draw(image<T> * img, size_t x, size_t y) {
		for (size_t i = 0; i < img -> height(); i++)
			for (size_t j = 0; j < img -> width(); j++)
				blend(img -> get(j, i), x + j, y + i);
	}

	template <pixel_type T>
	void draw(image<T> * img, size_t sx, size_t sy,
			  size_t dx, size_t dy, size_t w, size_t h) {
		for (size_t i = 0; i < h; i++)
			for (size_t j = 0; j < w; j++)
				blend(img -> get(j + sx, i + sy), dx + j, dy + i);
	}

	void draw(pixel p, size_t x, size_t y, size_t w, size_t h) {
		for (size_t i = 0; i < h; i++)
			for (size_t j = 0; j < w; j++)
				blend(p, x + j, y + i);
	}

	image<P> copy() {
		uint8_t * data = new uint8_t[size];
		for (size_t i = 0; i < size; i++) data[i] = this -> data[i];
		return image<P>(data, w, h);
	}

	image<P> copy(size_t x, size_t y, size_t w, size_t h) {
		image<P> temp(new uint8_t[w * h], w, h);
		for (size_t i = 0; i < h; i++)
			for (size_t j = 0; j < w; j++)
				temp.set(get(j + x, i + y), i, j);
	}

	void free() { delete[] data; data = nullptr; }

	size_t width() { return w; }
	size_t height() { return h; }

	uint8_t * raw() { return data; }

	template <pixel_type T>
	uint8_t * to() {
		if (T == P) return data;
		uint8_t * buffer = new uint8_t[w * h * T];
		if (T == pixel_type::rgb) {
			for (size_t i = 0; i < w * h; i++) {
				buffer[i * 3] = data[i * 4];
				buffer[i * 3 + 1] = data[i * 4 + 1];
				buffer[i * 3 + 2] = data[i * 4 + 2];
			}
		} else {
			for (size_t i = 0; i < w * h; i++) {
				buffer[i * 4] = data[i * 3];
				buffer[i * 4 + 1] = data[i * 3 + 1];
				buffer[i * 4 + 2] = data[i * 3 + 2];
				buffer[i * 4 + 3] = 0xFF;
			}
		}
		return buffer;
	}

};

struct theme {
	image<rgb> * board_w, * board_b;
	image<rgba> * bk, * bq, * bb, * bn, * br, * bp;
	image<rgba> * wk, * wq, * wb, * wn, * wr, * wp;
	pixel highlight = 0;
};

void free(theme & t) {
	if (t.board_w) t.board_w -> free(); t.board_w = nullptr;
	if (t.board_b) t.board_b -> free(); t.board_b = nullptr;
	if (t.bk) t.bk -> free(); t.bk = nullptr;
	if (t.bq) t.bq -> free(); t.bq = nullptr;
	if (t.br) t.br -> free(); t.br = nullptr;
	if (t.bb) t.bb -> free(); t.bb = nullptr;
	if (t.bn) t.bn -> free(); t.bn = nullptr;
	if (t.bp) t.bp -> free(); t.bp = nullptr;
	if (t.wk) t.wk -> free(); t.wk = nullptr;
	if (t.wq) t.wq -> free(); t.wq = nullptr;
	if (t.wr) t.wr -> free(); t.wr = nullptr;
	if (t.wb) t.wb -> free(); t.wb = nullptr;
	if (t.wn) t.wn -> free(); t.wn = nullptr;
	if (t.wp) t.wp -> free(); t.wp = nullptr;
}

void place(image<rgb> & board, char piece, size_t f, size_t r,
		   theme & t, perspective p) {
	const size_t x = (p ? f : 7 - f) * t.wp -> width();
	const size_t y = (p ? 8 - r : r - 1) * t.wp -> height();
	switch (piece) {
		case 'P': board.draw(t.wp, x, y); break;
		case 'N': board.draw(t.wn, x, y); break;
		case 'B': board.draw(t.wb, x, y); break;
		case 'R': board.draw(t.wr, x, y); break;
		case 'Q': board.draw(t.wq, x, y); break;
		case 'K': board.draw(t.wk, x, y); break;
		case 'p': board.draw(t.bp, x, y); break;
		case 'n': board.draw(t.bn, x, y); break;
		case 'b': board.draw(t.bb, x, y); break;
		case 'r': board.draw(t.br, x, y); break;
		case 'q': board.draw(t.bq, x, y); break;
		case 'k': board.draw(t.bk, x, y); break;
	}
}

/// generate a png diagram from a FEN string
image<rgb> diagram(string fen, theme t, perspective p = WHITE) {
	image<rgb> board = (p ? t.board_w : t.board_b) -> copy();
	for (size_t i = 0, r = 8, f = A; i < fen.size(); i++) {
		if (fen[i] == ' ' || fen[i] == '+' || fen[i] == '_') break;
		if (fen[i] == '/') {
			if (r == 0) return board;
			r--; f = A;
			continue;
		}
		if (fen[i] >= '1' && fen[i] <= '8') {
			f += fen[i] - '0';
			continue;
		}
		if (f > H || r == 0) return board;
		place(board, fen[i], f++, r, t, p);
	}
	return board;
}

bool en_passant(char board[8][8], player turn, uint8_t * move) {
	if (board[move[1]][move[0]] != 'P' && board[move[1]][move[0]] != 'p')
		return false;
	// files must be shifted by 1:
	if (move[0] != move[2] - 1 && move[0] != move[2] + 1) return false;
	// starting ranks must be 4th for black and 5th for white,
	// ending ranks must be 3rd for black and 6th for white:
	if (turn == WHITE) {
		if (move[1] != 4 || move[3] != 5) return false;
	} else { // BLACK
		if (move[1] != 3 || move[3] != 2) return false;
	}
	return true; // en-passant!
}

bool short_castle(char board[8][8], player turn, uint8_t * move) {
	if (board[move[1]][move[0]] != 'K' && board[move[1]][move[0]] != 'k')
		return false;
	if (turn == WHITE) { // e1g1
		if (move[0] != E || move[1] != 0 || move[2] != G || move[3] != 0)
			return false;
	} else { // BLACK e8g8
		if (move[0] != E || move[1] != 7 || move[2] != G || move[3] != 7)
			return false;
	}
	return true; // short castle!
}

bool long_castle(char board[8][8], player turn, uint8_t * move) {
	if (board[move[1]][move[0]] != 'K' && board[move[1]][move[0]] != 'k')
		return false;
	if (turn == WHITE) { // e1c1
		if (move[0] != E || move[1] != 0 || move[2] != C || move[3] != 0)
			return false;
	} else { // BLACK e8c8
		if (move[0] != E || move[1] != 7 || move[2] != C || move[3] != 7)
			return false;
	}
	return true; // long castle!
}

vector<image<rgb>> diagrams(string moves, theme & t, perspective p = WHITE) {
	char l = moves[moves.length() - 1];
	while (l == ';' || l == ' ' || l == '\t' || l == '\n' || l == '\r') {
		moves.pop_back(); l = moves[moves.length() - 1];
	}
	const size_t side = t.wp -> width();
	vector<image<rgb>> frames;
	char board[8][8] = { // reversed y for initialization
		{ 'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' },
		{ 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p' },
		{ 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' },
	};
	// lit it going to contain highlighted squares:
	image<rgb> lit = (p ? t.board_w : t.board_b) -> copy();
	for (size_t i = 0; i < 8; i++) // ranks start at 1
		for (size_t j = 0; j < 8; j++)
			place(lit, board[i][j], j, i + 1, t, p);
	frames.push_back(lit); // push starting position
	lit = lit.copy();
	// unlit is NOT highlighted for fast frame swapping:
	image<rgb> unlit = lit.copy();
	uint8_t move[4]; uint8_t m = 0;
	player turn = WHITE; char promotion = ' ';
	size_t x, y, py;
	for (size_t i = 0; i < moves.length(); i++) {
		switch (moves[i]) {
			case ';': case ' ': case '\t': case '\n': case '\r':
				frames.push_back(lit);
				// frame swapping to start with an unlit board,
				// identical to the previous frame:
				lit = unlit;
				unlit = unlit.copy();
				turn = !turn;
				continue;
			break;
			// switch is faster than if statements due to jump table:
			case 'a': case 'b': case 'c': case 'd':
			case 'e': case 'f': case 'g': case 'h':
				move[m++] = moves[i] - 'a'; break;
			case 'A': case 'B': case 'C': case 'D':
			case 'E': case 'F': case 'G': case 'H':
				move[m++] = moves[i] - 'A'; break;
			case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8':
				move[m++] = moves[i] - '0' - 1; break;
			// en-passant for white, test: e2e4;d7d5;e4e5;f7f5;e5f6
			// en-passant for black, test: e2e4;d7d5;e4e5;d5d4;c2c4;d4c3;b2c3;
		}
		if (m == 4) { // move parsing complete:
			m = 0;
			if (i + 2 < moves.length() && moves[i + 1] == '=') {
				promotion = moves[i + 2];
				switch (promotion) {
					case 'Q': case 'R': case 'B': case 'N': break;
					case 'q': case 'r': case 'b': case 'n': break;
					default: promotion = ' '; break;
				}
				i += 2;
			} else promotion = ' ';
			if (en_passant(board, turn, move)) {
				// vacate the en-passant capture square:
				py = move[3] + (turn ? - 1 : 1);
				board[py][move[2]] = ' ';
				x = (p ? move[2] : 7 - move[2]) * side;
				y = (p ? 7 - py : py) * side;
				lit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
				unlit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			} else if (short_castle(board, turn, move)) {
				// vacate the rook's source square (h1 or h8):
				py = turn ? 0 : 7; // rook's source rank
				board[py][H] = ' ';
				x = (p ? H : 7 - H) * side;
				y = (p ? 7 - py : py) * side;
				lit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
				unlit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
				// draw the rook highlights:
				lit.draw(t.highlight, x, y, side, side); // source
				x = (p ? F : 7 - F) * side;
				y = (p ? 7 - py : py) * side;
				lit.draw(t.highlight, x, y, side, side); // destination
				// move the rook to the destination square (f1 or f8):
				char rook = turn == WHITE ? 'R' : 'r';
				board[py][F] = rook;
				place(lit, rook, F, py + 1, t, p);
				place(unlit, rook, F, py + 1, t, p);
			} else if (long_castle(board, turn, move)) {
				// vacate the rook's source square (a1 or a8):
				py = turn ? 0 : 7; // rook's source rank
				board[py][A] = ' ';
				x = (p ? A : 7 - A) * side;
				y = (p ? 7 - py : py) * side;
				lit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
				unlit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
				// draw the rook highlights:
				lit.draw(t.highlight, x, y, side, side); // source
				x = (p ? D : 7 - D) * side;
				y = (p ? 7 - py : py) * side;
				lit.draw(t.highlight, x, y, side, side); // destination
				// move the rook to the destination square (d1 or d8):
				char rook = turn == WHITE ? 'R' : 'r';
				board[py][D] = rook;
				place(lit, rook, D, py + 1, t, p);
				place(unlit, rook, D, py + 1, t, p);
			}
			// get the piece
			char piece = promotion == ' ' ? board[move[1]][move[0]] : promotion;
			// vacate the source square and draw the highlight
			board[move[1]][move[0]] = ' ';
			x = (p ? move[0] : 7 - move[0]) * side;
			y = (p ? 7 - move[1] : move[1]) * side;
			lit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			unlit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			lit.draw(t.highlight, x, y, side, side);
			// vacate the destination square and draw the highlight
			x = (p ? move[2] : 7 - move[2]) * side;
			y = (p ? 7 - move[3] : move[3]) * side;
			lit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			unlit.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			lit.draw(t.highlight, x, y, side, side);
			// draw the piece
			board[move[3]][move[2]] = piece;
			place(lit, piece, move[2], move[3] + 1, t, p);
			place(unlit, piece, move[2], move[3] + 1, t, p);
		}
	}
	frames.push_back(lit);
	unlit.free();
	return frames;
}

#endif
