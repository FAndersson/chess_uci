#pragma once 

#include <string>
#include <vector>

namespace chess {
namespace uci {

/// An analyzed line is a suggested game continuation from the engine, together with the engine evaluation of that line.
struct Analyzed_line
{
	/// Sequence of suggested best moves in long algebraic notation.
	std::vector<std::string> line;
	/// Evaluation of the suggested game continuation.
	Evaluation evaluation;
};

} // namespace uci
} // namespace chess
