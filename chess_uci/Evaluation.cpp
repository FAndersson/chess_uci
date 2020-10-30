#include "Evaluation.h"

#include <stdexcept>

namespace chess {
namespace uci {

namespace {

std::string centi_pawns_to_string(int16_t cp)
{
	if (cp == 0)
		return "0.00";
	std::string str = std::to_string(cp / 100) + ".";
	int16_t decimals = cp % 100;
	if (decimals < 0)
		decimals = -decimals;
	if (decimals < 10) {
		if (decimals == 0)
			str += "00";
		else
			str += "0" + std::to_string(decimals);
	} else {
		str += std::to_string(decimals);
	}
	if (cp > 0)
		return "+" + str;
	else
		return str;
}

} // Anonymous namespace

std::string to_string(const Evaluation& evaluation)
{
	if (evaluation.centi_pawns.has_value()) {
		return centi_pawns_to_string(*evaluation.centi_pawns);
	}

	if (evaluation.centi_pawns_lower_bound.has_value()) {
		return ">= " + centi_pawns_to_string(*evaluation.centi_pawns_lower_bound);
	}

	if (evaluation.centi_pawns_upper_bound.has_value()) {
		return "<= " + centi_pawns_to_string(*evaluation.centi_pawns_upper_bound);
	}

	if (evaluation.white_can_mate_in.has_value()) {
		return "White mate in " + std::to_string(*evaluation.white_can_mate_in);
	}

	if (evaluation.black_can_mate_in.has_value()) {
		return "Black mate in " + std::to_string(*evaluation.black_can_mate_in);
	}

	throw std::runtime_error("Cannot convert evaluation to string: No evaluation info");
}

} // namespace uci
} // namespace chess
