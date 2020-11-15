#define CATCH_CONFIG_MAIN

#include "chess_uci/Evaluation.h"

#include <catch2/catch.hpp>

namespace chess {
namespace uci {

TEST_CASE("chess::uci::Evaluation.Evaluation to string", "[evaluation], [chess], [uci]")
{
	{
		Evaluation evaluation;
		evaluation.centi_pawns = 100;
		CHECK(to_string(evaluation) == "+1.00");
		evaluation.centi_pawns = 20;
		CHECK(to_string(evaluation) == "+0.20");
		evaluation.centi_pawns = 8;
		CHECK(to_string(evaluation) == "+0.08");
		evaluation.centi_pawns = -131;
		CHECK(to_string(evaluation) == "-1.31");
		evaluation.centi_pawns = 0;
		CHECK(to_string(evaluation) == "0.00");
	}

	{
		Evaluation evaluation;
		evaluation.centi_pawns_lower_bound = 234;
		CHECK(to_string(evaluation) == ">= +2.34");
	}

	{
		Evaluation evaluation;
		evaluation.centi_pawns_upper_bound = 234;
		CHECK(to_string(evaluation) == "<= +2.34");
	}

	{
		Evaluation evaluation;
		evaluation.white_can_mate_in = 3;
		CHECK(to_string(evaluation) == "White mate in 3");
	}

	{
		Evaluation evaluation;
		evaluation.black_can_mate_in = 15;
		CHECK(to_string(evaluation) == "Black mate in 15");
	}
}

} // namespace uci
} // namespace chess
