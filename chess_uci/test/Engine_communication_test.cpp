#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <boost/process.hpp>

#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace chess {
namespace uci {

TEST_CASE("chess::uci.Basic chess engine communication", "[communication], [chess], [uci]")
{
	// Start engine in a child process
	boost::process::ipstream engine_output;
	boost::process::opstream engine_input;
	std::unique_ptr<boost::process::child> engine_process;
	SECTION("Communication with a dummy engine")
	{
		engine_process = std::make_unique<boost::process::child>("./dummy_engine", boost::process::std_out > engine_output, boost::process::std_in < engine_input);
	}
	SECTION("Communication with Stockfish")
	{
		engine_process = std::make_unique<boost::process::child>("/usr/games/stockfish", boost::process::std_out > engine_output, boost::process::std_in < engine_input);
	}

	// Send uci command and make sure that we get a uciok reply
	engine_input << "uci\n"
				 << std::flush;
	std::string reply;
	bool uciok_reply = false;
	while (std::getline(engine_output, reply))
		if (reply.substr(0, 5) == "uciok") {
			uciok_reply = true;
			break;
		}
	REQUIRE(uciok_reply == true);

	// Check if engine is ready
	engine_input << "isready\n"
				 << std::flush;
	std::getline(engine_output, reply);
	REQUIRE(reply == "readyok");

	// Send commands for setting up a new game
	engine_input << "ucinewgame\n"
				 << std::flush;
	engine_input << "position startpos\n"
				 << std::flush;

	// Send command for letting the engine calculate for a while
	engine_input << "go\n"
				 << std::flush;
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);
	engine_input << "stop\n"
				 << std::flush;

	// Make sure that we get a bestmove output from the engine
	bool bestmove_reply = false;
	while (std::getline(engine_output, reply))
		if (reply.substr(0, 8) == "bestmove") {
			bestmove_reply = true;
			break;
		}
	REQUIRE(bestmove_reply == true);

	engine_input << "quit\n"
				 << std::flush;

	engine_process->wait();
}

} // namespace uci
} // namespace chess
