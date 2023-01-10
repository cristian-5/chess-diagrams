
#ifndef DIAGRAM_HPP
#define DIAGRAM_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>

const bool WHITE = true;
const bool BLACK = false;
using perspective = bool;
using pixel = std::uint32_t;
using byte = std::uint8_t;

enum files { A, B, C, D, E, F, G, H };

enum pixel_type: unsigned { rgb = 3, rgba = 4 };
template <pixel_type P>
class image {

	private:

	byte * data;
	size_t size, w, h;

	public:

	image(std::string name, size_t h, size_t w) {
		std::ifstream file(name.c_str(), std::ios::binary);
		if (!file.good()) return;
		file.seekg(0, std::ios::end);
		this -> size = file.tellg();
		file.seekg(0, std::ios::beg);
		this -> data = new byte[this -> size];
		file.read((char *) this -> data, this -> size);
		file.close();
		this -> w = w; this -> h = h;
	}

	image(std::string name, size_t s) {
		std::ifstream file(name.c_str(), std::ios::binary);
		if (!file.good()) return;
		file.seekg(0, std::ios::end);
		this -> size = file.tellg();
		file.seekg(0, std::ios::beg);
		this -> data = new byte[size];
		file.read((char *) this -> data, size);
		file.close();
		this -> w = s; this -> h = s;
	}

	image(byte * data, size_t w, size_t h) {
		this -> data = data;
		this -> size = h * w * P;
		this -> w = w; this -> h = h;
	}

	image(byte * data, size_t s) {
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
		const byte a = p & 0xFF;
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
	void draw(image<T> * img, size_t sx, size_t sy, size_t dx, size_t dy, size_t w, size_t h) {
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
		byte * data = new byte[size];
		for (size_t i = 0; i < size; i++) data[i] = this -> data[i];
		return image<P>(data, w, h);
	}

	image<P> copy(size_t x, size_t y, size_t w, size_t h) {
		image<P> temp(new byte[w * h], w, h);
		for (size_t i = 0; i < h; i++)
			for (size_t j = 0; j < w; j++)
				temp.set(get(j + x, i + y), i, j);
	}

	void free() { delete[] data; data = nullptr; }

	size_t width() { return w; }
	size_t height() { return h; }

	byte * raw() { return data; }

	template <pixel_type T>
	byte * to() {
		if (T == P) return data;
		byte * buffer = new byte[w * h * T];
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

void draw(image<rgb> & board, char piece, size_t f, size_t r, theme & t, perspective p) {
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
image<rgb> diagram(std::string fen, theme t, perspective p = WHITE) {
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
		draw(board, fen[i], f++, r, t, p);
	}
	return board;
}

std::vector<image<rgb>> diagrams(std::string moves, theme & t, perspective p = WHITE) {
	char l = moves[moves.length() - 1];
	while (l == ';' || l == ' ' || l == '\t' || l == '\n' || l == '\r') {
		moves.pop_back(); l = moves[moves.length() - 1];
	}
	const size_t side = t.wp -> width();
	std::vector<image<rgb>> frames;
	char board[8][8] = { // reversed y for indexing
		{ 'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' },
		{ 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{ 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p' },
		{ 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' },
	};
	image<rgb> current = (p ? t.board_w : t.board_b) -> copy();
	for (size_t i = 0; i < 8; i++) // ranks start at 1
		for (size_t j = 0; j < 8; j++)
			draw(current, board[i][j], j, i + 1, t, p);
	frames.push_back(current); // push starting position
	current = current.copy();
	image<rgb> last = current.copy();
	byte move[4]; char promotion = ' '; byte m = 0; bool ep = false;
	for (size_t i = 0, turn = 0; i < moves.length(); i++) {
		switch (moves[i]) {
			case ';': case ' ': case '\t': case '\n': case '\r':
				frames.push_back(current);
				current = last;
				last = last.copy();
				turn++;
				continue;
			break;
			// switch is faster than if statements due to jump table
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
				move[m++] = moves[i] - 'a'; break;
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
				move[m++] = moves[i] - 'A'; break;
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
				move[m++] = moves[i] - '0' - 1; break;
			case '$': ep = true; continue; // en-passant for the next move
			// en-passant for white, test: e2e4;d7d5;e4e5;f7f5;$e5f6
			// en-passant for black, test: e2e4;d7d5;e4e5;d5d4;c2c4;$d4c3;b2c3;c7c6
			/*case 'K': // kingside castle white
			break;
			case 'k': // kingside castle black
			break;
			case 'Q': // queenside castle white
			break;
			case 'q': // queenside castle black
			break;*/
		}
		if (m == 4) {
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
			// get the piece
			char piece = promotion == ' ' ? board[move[1]][move[0]] : promotion;
			// vacate the source square and draw the highlight
			board[move[1]][move[0]] = ' ';
			size_t x = (p ? move[0] : 7 - move[0]) * side;
			size_t y = (p ? 7 - move[1] : move[1]) * side;
			current.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			last.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			current.draw(t.highlight, x, y, side, side);
			// vacate the destination square and draw the highlight
			x = (p ? move[2] : 7 - move[2]) * side;
			y = (p ? 7 - move[3] : move[3]) * side;
			current.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			last.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
			current.draw(t.highlight, x, y, side, side);
			// if en-passant, vacate the captured pawn
			if (ep) {
				const byte py = move[3] + ((turn & 1) ? 1 : - 1);
				board[py][move[2]] = ' ';
				x = (p ? move[2] : 7 - move[2]) * side;
				y = (p ? 7 - py : py) * side;
				current.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
				last.draw((p ? t.board_w : t.board_b), x, y, x, y, side, side);
				ep = false;
			}
			// draw the piece
			board[move[3]][move[2]] = piece;
			draw(current, piece, move[2], move[3] + 1, t, p);
			draw(last, piece, move[2], move[3] + 1, t, p);
		}
	}
	frames.push_back(current);
	last.free();
	return frames;
}

#endif
