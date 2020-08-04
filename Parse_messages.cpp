#include "Parse_messages.h"

#include <cstring>
#include <stdexcept>
#include <string>

namespace chess {
namespace uci {

Info parse_info(const std::string& info_str)
{
	Info info;

	// Parse for pv info
	if (size_t pos = info_str.rfind("pv"); pos != std::string::npos) {
		info.sequence_of_moves = impl::parse_pv(info_str.substr(pos));
	}

	// Parse line index
	if (size_t pos = info_str.find("multipv"); pos != std::string::npos) {
		try {
			info.line_index = std::stoi(info_str.substr(pos + 8));
		} catch (std::exception& e) {
			throw std::runtime_error(std::string("Error parsing multipv: ") + e.what());
		}
	}

	// Parse evaluation
	if (size_t pos = info_str.find("score"); pos != std::string::npos) {
		info.evaluation = impl::parse_score(info_str.substr(pos));
	}

	// Parse number of nodes searched
	if (size_t pos = info_str.find("nodes"); pos != std::string::npos) {
		try {
			info.nodes = std::stoi(info_str.substr(pos + 6));
		} catch (std::exception& e) {
			throw std::runtime_error(std::string("Error parsing nodes: ") + e.what());
		}
	}

	// Parse calculation time
	if (size_t pos = info_str.find("time"); pos != std::string::npos) {
		try {
			info.time = std::stoi(info_str.substr(pos + 5));
		} catch (std::exception& e) {
			throw std::runtime_error(std::string("Error parsing time: ") + e.what());
		}
	}

	return info;
}

namespace impl {

std::vector<std::string> tokenize(const std::string& string, char delimeter)
{
	std::vector<std::string> tokens;
	// strtok expects non-const data
	std::string string_copy = string;
	char* p = std::strtok(string_copy.data(), &delimeter);
	while (p) {
		tokens.push_back(p);
		p = std::strtok(NULL, &delimeter);
	}
	return tokens;
}

Evaluation parse_score(const std::string& score)
{
	Evaluation evaluation;
	if (score.substr(0, 6) != "score ")
		throw std::runtime_error("Error parsing score: Score string is malformed");

	// First handle mating scores
	if (score.substr(6, 4) == "mate") {
		int mate_in_moves = stoi(score.substr(11));
		if (mate_in_moves == 0)
			throw std::runtime_error("Error parsing score: Failed to parse number of moves to mate");
		if (mate_in_moves > 0) {
			evaluation.white_can_mate_in = mate_in_moves;
			return evaluation;
		} else {
			evaluation.black_can_mate_in = -mate_in_moves;
			return evaluation;
		}
	}

	// Parse centi pawns evaluation
	if (score.substr(6, 2) != "cp")
		throw std::runtime_error("Error parsing score: Failed to parse centi pawns evaluation");

	try {
		size_t pos = 0;
		int centi_pawns_evaluation = std::stoi(score.substr(9), &pos);
		if (score.size() > 9 + pos) {
			// Parse lower/upper bound
			if (score.substr(9 + pos + 1) == "lowerbound") {
				evaluation.centi_pawns_lower_bound = centi_pawns_evaluation;
			} else {
				if (score.substr(9 + pos + 1) == "upperbound")
					evaluation.centi_pawns_upper_bound = centi_pawns_evaluation;
				else
					evaluation.centi_pawns = centi_pawns_evaluation;
			}
		} else {
			evaluation.centi_pawns = centi_pawns_evaluation;
		}
	} catch (std::exception& e) {
		throw std::runtime_error(std::string("Error parsing score cp entry: ") + e.what());
	}

	return evaluation;
}

std::vector<std::string> parse_pv(const std::string& pv)
{
	// Parse pv
	if (pv.substr(0, 3) != "pv ")
		throw std::runtime_error("Error parsing pv: pv string is malformed");

	// Assume rest of string contains the engine line
	return tokenize(pv.substr(3), ' ');
}

} // namespace impl

} // namespace uci
} // namespace chess
