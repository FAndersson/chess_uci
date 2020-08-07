#define CATCH_CONFIG_MAIN

#include "Parse_messages.h"

#include <catch.hpp>

namespace chess {
namespace uci {

TEST_CASE("chess::uci::Parse_messages.Tokenize string", "[parsing], [chess], [uci]")
{
	std::vector<std::string> tokens = impl::tokenize("My little string", ' ');
	REQUIRE(tokens.size() == 3);
	CHECK(tokens[0] == "My");
	CHECK(tokens[1] == "little");
	CHECK(tokens[2] == "string");

	// Ignore more than one delimeter in sequence
	tokens = impl::tokenize("My     little string", ' ');
	REQUIRE(tokens.size() == 3);
	CHECK(tokens[0] == "My");
	CHECK(tokens[1] == "little");
	CHECK(tokens[2] == "string");

	// Ignore delimeter at the start and end of the string
	tokens = impl::tokenize("\nMy\nstring\nlines", '\n');
	REQUIRE(tokens.size() == 3);
	CHECK(tokens[0] == "My");
	CHECK(tokens[1] == "string");
	CHECK(tokens[2] == "lines");

	tokens = impl::tokenize("My\nstring\nlines\n", '\n');
	REQUIRE(tokens.size() == 3);
	CHECK(tokens[0] == "My");
	CHECK(tokens[1] == "string");
	CHECK(tokens[2] == "lines");
}

TEST_CASE("chess::uci::Parse_messages.Parse score", "[parsing], [chess], [uci]")
{
	// Handle regular scores
	Evaluation evaluation = impl::parse_score("score cp 114");
	REQUIRE(evaluation.centi_pawns.has_value());
	CHECK(*evaluation.centi_pawns == 114);
	CHECK(!evaluation.centi_pawns_lower_bound.has_value());
	CHECK(!evaluation.centi_pawns_upper_bound.has_value());
	CHECK(!evaluation.white_can_mate_in.has_value());
	CHECK(!evaluation.black_can_mate_in.has_value());
	evaluation = impl::parse_score("score cp -4");
	REQUIRE(evaluation.centi_pawns.has_value());
	CHECK(*evaluation.centi_pawns == -4);
	CHECK(!evaluation.centi_pawns_lower_bound.has_value());
	CHECK(!evaluation.centi_pawns_upper_bound.has_value());
	CHECK(!evaluation.white_can_mate_in.has_value());
	CHECK(!evaluation.black_can_mate_in.has_value());

	// Handle mating scores
	evaluation = impl::parse_score("score mate 3");
	CHECK(!evaluation.centi_pawns.has_value());
	CHECK(!evaluation.centi_pawns_lower_bound.has_value());
	CHECK(!evaluation.centi_pawns_upper_bound.has_value());
	REQUIRE(evaluation.white_can_mate_in.has_value());
	CHECK(*evaluation.white_can_mate_in == 3);
	CHECK(!evaluation.black_can_mate_in.has_value());
	evaluation = impl::parse_score("score mate -23");
	CHECK(!evaluation.centi_pawns.has_value());
	CHECK(!evaluation.centi_pawns_lower_bound.has_value());
	CHECK(!evaluation.centi_pawns_upper_bound.has_value());
	CHECK(!evaluation.white_can_mate_in.has_value());
	REQUIRE(evaluation.black_can_mate_in.has_value());
	CHECK(*evaluation.black_can_mate_in == 23);

	// Handle lower/upper bounds
	evaluation = impl::parse_score("score cp 10 lowerbound");
	CHECK(!evaluation.centi_pawns.has_value());
	REQUIRE(evaluation.centi_pawns_lower_bound.has_value());
	CHECK(*evaluation.centi_pawns_lower_bound == 10);
	CHECK(!evaluation.white_can_mate_in.has_value());
	CHECK(!evaluation.black_can_mate_in.has_value());
	evaluation = impl::parse_score("score cp 14 upperbound");
	CHECK(!evaluation.centi_pawns.has_value());
	REQUIRE(evaluation.centi_pawns_upper_bound.has_value());
	CHECK(*evaluation.centi_pawns_upper_bound == 14);
	CHECK(!evaluation.white_can_mate_in.has_value());
	CHECK(!evaluation.black_can_mate_in.has_value());
}

TEST_CASE("chess::uci::Parse_messages.Parse pv", "[parsing], [chess], [uci]")
{
	std::vector<std::string> line = impl::parse_pv("pv e2e3");
	REQUIRE(line.size() == 1);
	CHECK(line[0] == "e2e3");

	line = impl::parse_pv("pv d2d4 c7c6 g1f3 d7d5 c2c3 g8f6");
	REQUIRE(line.size() == 6);
	CHECK(line[0] == "d2d4");
	CHECK(line[1] == "c7c6");
	CHECK(line[2] == "g1f3");
	CHECK(line[3] == "d7d5");
	CHECK(line[4] == "c2c3");
	CHECK(line[5] == "g8f6");
}

TEST_CASE("chess::uci::Parse_messages.Parse info", "[parsing], [chess], [uci]")
{
	Info info = parse_info("info depth 1 seldepth 1 multipv 1 score cp 114 nodes 20 nps 10000 tbhits 0 time 2 pv e2e3");
	REQUIRE(info.evaluation.has_value());
	REQUIRE(info.evaluation->centi_pawns.has_value());
	CHECK(*info.evaluation->centi_pawns == 114);
	REQUIRE(info.line_index.has_value());
	CHECK(info.line_index == 0);
	REQUIRE(info.nodes.has_value());
	CHECK(info.nodes == 20);
	REQUIRE(info.sequence_of_moves.has_value());
	CHECK(*info.sequence_of_moves == std::vector<std::string>{"e2e3"});
	REQUIRE(info.time.has_value());
	CHECK(*info.time == 2);

	info = parse_info("info depth 7 seldepth 8 multipv 1 score cp 45 upperbound nodes 3036 nps 233538 tbhits 0 time 13 pv d2d4 c7c6");
	REQUIRE(info.evaluation.has_value());
	REQUIRE(info.evaluation->centi_pawns.has_value());
	CHECK(*info.evaluation->centi_pawns == 45);
	REQUIRE(info.line_index.has_value());
	CHECK(info.line_index == 0);
	REQUIRE(info.nodes.has_value());
	CHECK(info.nodes == 3036);
	REQUIRE(info.sequence_of_moves.has_value());
	CHECK(*info.sequence_of_moves == std::vector<std::string>{"d2d4", "c7c6"});
	REQUIRE(info.time.has_value());
	CHECK(*info.time == 13);

	info = parse_info("info depth 5 seldepth 5 multipv 10 score cp 26 nodes 3079 nps 146619 tbhits 0 time 21 pv b2b3 e7e6 e2e3");
	REQUIRE(info.evaluation.has_value());
	REQUIRE(info.evaluation->centi_pawns.has_value());
	CHECK(*info.evaluation->centi_pawns == 26);
	REQUIRE(info.line_index.has_value());
	CHECK(info.line_index == 9);
	REQUIRE(info.nodes.has_value());
	CHECK(info.nodes == 3079);
	REQUIRE(info.sequence_of_moves.has_value());
	CHECK(*info.sequence_of_moves == std::vector<std::string>{"b2b3", "e7e6", "e2e3"});
	REQUIRE(info.time.has_value());
	CHECK(*info.time == 21);
}

} // namespace uci
} // namespace chess
