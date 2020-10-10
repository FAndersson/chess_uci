#define CATCH_CONFIG_MAIN

#include "Engine.h"

#include <catch.hpp>

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
	REQUIRE((lines.front().line.size() == 1));
	CHECK((lines.front().line.front() == "e2e4"));
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

} // namespace uci
} // namespace chess
