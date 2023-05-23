
#include <iostream>
#include <vector>
#include "diagram.hpp"
#include "lodepng.hpp"
#include "route66.hpp"
#define MSF_GIF_IMPL
#include "msf_gif.h"

theme full, theme50;
std::vector<theme> themes;
lodepng::State state;

enum themes50: size_t { bubble, wooden, nature, iceage, grapes };
pixel highlights[] = { 0xffd10066ul, 0x3cd2a066ul, 0xffd10073ul, 0x64ff6380ul, 0x2096f480ul };
const std::vector<std::string> theme50_names = { "bubble", "wooden", "nature", "iceage", "grapes" };

template <perspective P>
int fen(route66::request & request, std::ostream & headers, std::ostream & contents);
template <perspective P, size_t T>
int pgn(route66::request & request, std::ostream & headers, std::ostream & contents);

int main(int argc, char * argv[]) {

	full = {
		.board_w = new image<rgb>("bin/100/board_w.bin", 800),
		.board_b = new image<rgb>("bin/100/board_b.bin", 800),
		.bk = new image<rgba>("bin/100/bk.bin", 100),
		.bq = new image<rgba>("bin/100/bq.bin", 100),
		.bb = new image<rgba>("bin/100/bb.bin", 100),
		.bn = new image<rgba>("bin/100/bn.bin", 100),
		.br = new image<rgba>("bin/100/br.bin", 100),
		.bp = new image<rgba>("bin/100/bp.bin", 100),
		.wk = new image<rgba>("bin/100/wk.bin", 100),
		.wq = new image<rgba>("bin/100/wq.bin", 100),
		.wb = new image<rgba>("bin/100/wb.bin", 100),
		.wn = new image<rgba>("bin/100/wn.bin", 100),
		.wr = new image<rgba>("bin/100/wr.bin", 100),
		.wp = new image<rgba>("bin/100/wp.bin", 100),
	};

	theme50 = {
		.bk = new image<rgba>("bin/50/bk.bin", 50),
		.bq = new image<rgba>("bin/50/bq.bin", 50),
		.bb = new image<rgba>("bin/50/bb.bin", 50),
		.bn = new image<rgba>("bin/50/bn.bin", 50),
		.br = new image<rgba>("bin/50/br.bin", 50),
		.bp = new image<rgba>("bin/50/bp.bin", 50),
		.wk = new image<rgba>("bin/50/wk.bin", 50),
		.wq = new image<rgba>("bin/50/wq.bin", 50),
		.wb = new image<rgba>("bin/50/wb.bin", 50),
		.wn = new image<rgba>("bin/50/wn.bin", 50),
		.wr = new image<rgba>("bin/50/wr.bin", 50),
		.wp = new image<rgba>("bin/50/wp.bin", 50),
	};

	for (size_t i = 0; i < theme50_names.size(); i++) {
		theme50.board_w = new image<rgb>("bin/50/boards/" + theme50_names[i] + "_w.bin", 400);
		theme50.board_b = new image<rgb>("bin/50/boards/" + theme50_names[i] + "_b.bin", 400);
		theme50.highlight = highlights[i];
		themes.push_back(theme50);
	}

	state.info_raw.colortype = LCT_RGB;
	state.encoder.add_id = false;
	state.encoder.filter_strategy = LFS_ZERO;
	state.encoder.auto_convert = 0;
	state.encoder.zlibsettings.windowsize = 32768;
	state.encoder.zlibsettings.lazymatching = 1;
	state.encoder.zlibsettings.nicematch = 258;
	state.encoder.zlibsettings.minmatch = 3;
	state.encoder.zlibsettings.btype = 2;

	if (!route66::create(8080, "GET /fen/white/*", fen<WHITE>)) return 1;
	if (!route66::create(8080, "GET /fen/black/*", fen<BLACK>)) return 1;

	if (!route66::create(8080, "GET /pgn/bubble/white/*", pgn<WHITE, bubble>)) return 1;
	if (!route66::create(8080, "GET /pgn/bubble/black/*", pgn<BLACK, bubble>)) return 1;
	if (!route66::create(8080, "GET /pgn/wooden/white/*", pgn<WHITE, wooden>)) return 1;
	if (!route66::create(8080, "GET /pgn/wooden/black/*", pgn<BLACK, wooden>)) return 1;
	if (!route66::create(8080, "GET /pgn/nature/white/*", pgn<WHITE, nature>)) return 1;
	if (!route66::create(8080, "GET /pgn/nature/black/*", pgn<BLACK, nature>)) return 1;
	if (!route66::create(8080, "GET /pgn/iceage/white/*", pgn<WHITE, iceage>)) return 1;
	if (!route66::create(8080, "GET /pgn/iceage/black/*", pgn<BLACK, iceage>)) return 1;
	if (!route66::create(8080, "GET /pgn/grapes/white/*", pgn<WHITE, grapes>)) return 1;
	if (!route66::create(8080, "GET /pgn/grapes/black/*", pgn<BLACK, grapes>)) return 1;

	std::cout << "server ready at localhost:8080" << std::endl;

	route66::traffic();

	free(full);
	for (auto & t : themes) free(t);

	return 0;
}

template <perspective P>
int fen(route66::request & request, std::ostream & headers, std::ostream & contents) {
	image<rgb> board = diagram(request.uri.substr(11), full, P);
	std::vector<unsigned char> png;
	lodepng::encode(png, (unsigned char *) board.raw(), 800, 800, state);
	headers << property("Content-Type", "image/png");
	contents.write((char *) png.data(), png.size());
	return 200;
}

template <perspective P, size_t T>
int pgn(route66::request & request, std::ostream & headers, std::ostream & contents) {
	std::vector<image<rgb>> boards = diagrams(request.uri.substr(17), themes[T], P);
	MsfGifState gifstate;
	msf_gif_begin(& gifstate, 400, 400);
	for (auto & board : boards) {
		uint8_t * raw = board.to<rgba>();
		msf_gif_frame(& gifstate, raw, 80, 16, board.width() * 4);
		board.free(); delete[] raw;
	}
	MsfGifResult result = msf_gif_end(& gifstate);
	headers << property("Content-Type", "image/gif");
	contents.write((char *) result.data, result.dataSize);
	msf_gif_free(result);
	return 200;
}
