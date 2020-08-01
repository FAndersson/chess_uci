/**
 * \file Evaluation.h
 * \brief Basic types and functionality for working with engine evaluations.
 */

#pragma once

#include <optional>
#include <string>

namespace chess {
namespace uci {

/// Struct containing engine evaluation of a chess game state.
/// Only one of the entries in the struct should be set at the same time. The rest should be set to nullopt.
struct Evaluation
{
	/// Engine evaluation in centi pawns.
	std::optional<int16_t> centi_pawns;
	/// Engine evaluation in centi pawns when only a lower bound is known
	std::optional<int16_t> centi_pawns_lower_bound;
	/// Engine evaluation in centi pawns when only an upper bound is known
	std::optional<int16_t> centi_pawns_upper_bound;
	/// Engine has found a way for white to mate in at most this many moves.
	std::optional<uint8_t> white_can_mate_in;
	/// Engine has found a way for black to mate in at most this many moves.
	std::optional<uint8_t> black_can_mate_in;
};

/// Utility function to convert an engine evaluation to a human readable string.
std::string to_string(const Evaluation& evaluation);

} // namespace uci
} // namespace chess
