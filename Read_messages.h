/**
 * \file Read_messages.h
 * \brief Read messages sent by the engine to a stream.
 */

#pragma once

#include <istream>
#include <string>
#include <vector>

namespace chess {
namespace uci {

/**
 * Read messages received from the engine after the 'uci' command has been sent to the engine.
 *
 * The engine is required to send the message 'uciok\n' as the last reply to the 'uci\n' command.
 * Before that it is required to send 'id' and 'option' replies, and optionally can send any other
 * relevant messages. Each message is sent as a separate line (e.g. ending with the newline character).
 * All of these messages are read by this function. The newline character at the end of each message is
 * only used as a separator between messages, and hence carry no meaning so it's stripped from the
 * output of this function.
 *
 * \param stream Stream to which the engine writes its messages.
 * \return List of messages from the engine, without a final newline character. The last of these
 * messages is always 'uciok'.
 */
std::vector<std::string> read_uci_replies(std::istream& stream);

/**
 * Read messages received from the engine after the 'isready' command has been sent to the engine.
 *
 * The engine is required to send the message 'readyok\n' as the last reply to the 'isready\n' command.
 * Before that it can send any other messages as separate lines (e.g. ending with the newline character).
 * All of these messages are read by this function. The newline character at the end of each message is
 * only used as a separator between messages, and hence carry no meaning so it's stripped from the
 * output of this function.
 *
 * \param stream Stream to which the engine writes its messages.
 * \return List of messages from the engine, without a final newline character. The last of these
 * messages is always 'readyok'.
 */
std::vector<std::string> read_isready_replies(std::istream& stream);

/**
 * Read messages received from the engine after the 'go' command has been sent to the engine.
 *
 * The engine is required to send a message starting with 'bestmove' followed by info about the best
 * move found as the last reply to the 'go\n' command.
 * Before that it is required to send 'info' messages with final search information, and optionally can
 * send any other relevant messages before that. Each message is sent as a separate line (e.g. ending
 * with the newline character). All of these messages are read by this function. The newline character
 * at the end of each message is only used as a separator between messages, and hence carry no meaning
 * so it's stripped from the output of this function.
 *
 * \param stream Stream to which the engine writes its messages.
 * \return List of messages from the engine, without a final newline character. The last of these
 * messages always starts with 'bestmove'.
 */
std::vector<std::string> read_go_replies(std::istream& stream);

} // namespace uci
} // namespace chess
