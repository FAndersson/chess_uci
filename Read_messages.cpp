#include "Read_messages.h"

#include <istream>

namespace chess {
namespace uci {

namespace {

/// Continue to read lines from the given stream until the given
/// predicate is true for the last read line.
template<class UnaryPredicate>
const std::vector<std::string> read_lines_from_stream_until(std::istream& stream, UnaryPredicate p)
{
	std::vector<std::string> messages;
	std::string message;
	bool read_finished = false;
	while (!read_finished) {
		std::getline(stream, message);
		while (stream.eof()) {
			// Continue reading until we have read a full message
			std::getline(stream, message);
		}
		messages.emplace_back(std::move(message));
		read_finished = p(messages.back());
	}
	return messages;
}

} // Anonymous namespace

std::vector<std::string> read_uci_replies(std::istream& stream)
{
	return read_lines_from_stream_until(stream, [](const std::string& message) {
		return message == "uciok";
	});
}

std::vector<std::string> read_isready_replies(std::istream& stream)
{
	return read_lines_from_stream_until(stream, [](const std::string& message) {
		return message == "readyok";
	});
}

std::vector<std::string> read_go_replies(std::istream& stream)
{
	return read_lines_from_stream_until(stream, [](const std::string& message) {
		return message.substr(0, 8) == "bestmove";
	});
}

} // namespace uci
} // namespace chess
