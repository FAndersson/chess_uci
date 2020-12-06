/**
 * \file Parse_messages.h
 * \brief Functionality for parsing engine messages.
 */

#pragma once

#include "Evaluation.h"

#include <optional>
#include <string>
#include <vector>

namespace chess {
namespace uci {

/// Struct containing parsed info from an engine info message.
struct Info
{
	/// Sequence of future moves the engine suggest.
	std::optional<std::vector<std::string>> sequence_of_moves;
	/// When the engine suggests several future move sequences at the same time,
	/// this number gives the index of the sequence suggested in this info message.
	/// Uses zero-based indexing, e.g. 0 for the best continuation, 1 for the
	/// second best and so on.
	std::optional<uint8_t> line_index;
	/// Current evaluation, based on the suggested sequence of future moves.
	std::optional<Evaluation> evaluation;
	/// Number of nodes (game states) the engine has analyzed.
	std::optional<unsigned int> nodes;
	/// Time the engine has been calculating in milliseconds.
	std::optional<unsigned int> time;
};

/**
 * Parse an info message from the engine.
 *
 * \param info Info message from the engine.
 * \return Parsed info.
 * \throw std::runtime_error if parsing the info string fails unexpectedly.
 */
Info parse_info(const std::string& info);

namespace impl {

/**
 * Tokenize a string (split into parts) based on the given delimeter.
 *
 * Note: The empty string is not considered a token, so if the given delimeter is found at the
 * start/end of the string or in sequence within the string then those extra delimeters are
 * just skipped.
 *
 * \param string String that we want to tokenize.
 * \param delimeter Character at which we want to split the original string.
 * \param Array of parts from the original string after the split, with the
 * delimeter character removed.
 */
std::vector<std::string> tokenize(const std::string& string, char delimeter);

/**
 * Parse score part of an engine message.
 *
 * \param score Score info from the engine.
 * \return Parsed evaluation info.
 * \throw std::runtime_error if parsing the score string fails unexpectedly.
 */
Evaluation parse_score(const std::string& score);

/**
 * Parse pv part of an engine message.
 *
 * \param pv Line info from the engine.
 * \return Parsed line info as an sequence of moves in long algebraic notation.
 */
std::vector<std::string> parse_pv(const std::string& pv);

} // namespace impl

} // namespace uci
} // namespace chess
