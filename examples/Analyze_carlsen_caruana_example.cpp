#include "chess_uci/Engine.h"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	using namespace chess::uci;

	// Initiate engine running Stockfish, and specify that the engine should
	// output the 3 best lines
	Engine engine("/usr/games/stockfish", 3);

	// Setup game to analyze after the 10 first moves of the final rapid game
	// in the Carlsen-Caruana world championship match 2018
	std::string moves_lan = "e2e4 c7c5 g1f3 e7e6 c2c4 b8c6 d2d4 c5d4 f3d4 f8c5 d4c2 g8f6 b1c3 e8g8 c1e3 b7b6 f1e2 c8b7 e1g1 d8e7";
	engine.setup_game_from_moves(moves_lan);

	// Let engine calculate for 1 s
	engine.start_calculating();
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);
	engine.stop_calculating();

	// Print the best lines given by the engine and their evaluation
	for (const Analyzed_line& line : engine.get_top_suggested_move_sequences()) {
		std::cout << "Move sequence:";
		for (const std::string& move : line.moves)
			std::cout << " " << move;
		std::cout << std::endl;
		std::cout << "Evaluation: " << to_string(line.evaluation) << std::endl;
	}

	return 0;
}