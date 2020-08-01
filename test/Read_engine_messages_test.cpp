#define CATCH_CONFIG_MAIN

#include "Read_engine_messages.h"

#include <catch.hpp>

#include <sstream>

namespace chess {
namespace uci {

TEST_CASE("chess::uci::Read_engine_messages.Read uci messages", "[communication], [chess], [uci]")
{
	std::stringstream stream;

	// Write fake engine messages to the stream
	stream << "id name Fake engine\n";
	stream << "id author Santa\n";
	stream << "option name FavoriteColor value string\n";
	stream << "uciok\n";

	// Read engine messages
	std::vector<std::string> messages = read_uci_replies(stream);
	REQUIRE(messages.size() == 4);
	REQUIRE(messages[0] == "id name Fake engine");
	REQUIRE(messages[1] == "id author Santa");
	REQUIRE(messages[2] == "option name FavoriteColor value string");
	REQUIRE(messages[3] == "uciok");
}

TEST_CASE("chess::uci::Read_engine_messages.Read isready messages", "[communication], [chess], [uci]")
{
	std::stringstream stream;

	// Write fake engine messages to the stream
	stream << "Hello from engine\n";
	stream << "readyok\n";

	// Read engine messages
	std::vector<std::string> messages = read_isready_replies(stream);
	REQUIRE(messages.size() == 2);
	REQUIRE(messages[0] == "Hello from engine");
	REQUIRE(messages[1] == "readyok");
}

TEST_CASE("chess::uci::Read_engine_messages.Read go messages", "[communication], [chess], [uci]")
{
	std::stringstream stream;

	// Write fake engine messages to the stream
	stream << "info score 20 pv e2e4\n";
	stream << "bestmove e2e4\n";

	// Read engine messages
	std::vector<std::string> messages = read_go_replies(stream);
	REQUIRE(messages.size() == 2);
	REQUIRE(messages[0] == "info score 20 pv e2e4");
	REQUIRE(messages[1] == "bestmove e2e4");
}

} // namespace uci
} // namespace chess
