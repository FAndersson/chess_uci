#define CATCH_CONFIG_MAIN

#include "chess_uci/Engine.h"

#include <catch2/catch.hpp>

#include <chrono>
#include <thread>

namespace chess {
namespace uci {

TEST_CASE("chess::uci.Engine.Dummy engine", "[chess], [uci]")
{
	// Create an instance of the interface running the dummy engine, and make
	// sure that the interface calls works as expected
	Engine engine("./dummy_engine");

	engine.reset_game();

	engine.start_calculating();
	engine.stop_calculating();

	auto evaluation = engine.get_evaluation();
	REQUIRE(evaluation.centi_pawns.has_value());
	CHECK((*evaluation.centi_pawns == 30));
	const auto& lines = engine.get_top_suggested_move_sequences();
	REQUIRE((lines.size() == 1));
	REQUIRE((lines.front().moves.size() == 1));
	CHECK((lines.front().moves.front() == "e2e4"));
	REQUIRE(lines.front().evaluation.centi_pawns.has_value());
	CHECK((*lines.front().evaluation.centi_pawns == 30));
}

TEST_CASE("chess::uci.Engine.Stockfish basic", "[chess], [uci]")
{
	// Create an instance of the interface running Stockfish, and make
	// sure that the interface calls works as expected
	Engine engine("/usr/games/stockfish");

	engine.reset_game();

	engine.start_calculating();
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(500ms);
	engine.stop_calculating();

	const auto& lines = engine.get_top_suggested_move_sequences();
	REQUIRE((lines.size() == 1));
}

TEST_CASE("chess::uci.Engine.Stockfish number of expected lines", "[chess], [uci]")
{
	// Create an instance of the interface running Stockfish, and make
	// sure that the interface calls works as expected, and that the expected
	// number of best lines are calculated
	Engine engine("/usr/games/stockfish", 4);

	engine.reset_game();

	engine.start_calculating();
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(500ms);
	engine.stop_calculating();

	const auto& lines = engine.get_top_suggested_move_sequences();
	REQUIRE((lines.size() == 4));
}

TEST_CASE("chess::uci.Engine.Stockfish number of moves to mate", "[chess], [uci]")
{
	// Create an instance of the interface running Stockfish
	Engine engine("/usr/games/stockfish");
	// Setup a position from which white can mate in 5 moves, and verify that the
	// engine gives the expected number of moves to mate
	std::string fen = "8/8/8/4k3/8/8/6Q1/5R1K w - - 0 20";
	engine.setup_game_from_fen(fen);

	using namespace std::chrono_literals;
	engine.start_calculating(1s);

	std::this_thread::sleep_for(1s);
	engine.stop_calculating();

	// White should be able to mate in 5 moves
	Evaluation evaluation = engine.get_evaluation();
	REQUIRE(evaluation.white_can_mate_in.has_value());
	REQUIRE(evaluation.white_can_mate_in.value() == 5);

	const Analyzed_line& best_line = engine.get_top_suggested_move_sequences()[0];
	// The best line should contain 9 moves, 5 for white and four for black
	REQUIRE(best_line.moves.size() == 9);
}

} // namespace uci
} // namespace chess
