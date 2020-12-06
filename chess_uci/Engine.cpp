#include "Engine.h"

#include "Parse_messages.h"
#include "Read_messages.h"

#include <boost/process.hpp>

#include <cassert>

namespace chess {
namespace uci {

struct Engine_process_manager
{
	Engine_process_manager(const std::filesystem::path& engine_executable)
		: engine_child_process_(engine_executable.string(), boost::process::std_out > engine_to_host_, boost::process::std_in < host_to_engine_)
	{}

	/// Stream which the engine writes it's messages to.
	boost::process::pstream engine_to_host_;
	/// Stream which the engine reads commands from.
	boost::process::pstream host_to_engine_;

	/// Child process running the engine.
	boost::process::child engine_child_process_;
};

Engine::Engine(const std::filesystem::path& engine_executable, uint8_t num_best_lines, std::optional<uint16_t> max_elo_rating)
	: engine_process(std::make_unique<Engine_process_manager>(engine_executable))
	, num_best_lines_(num_best_lines)
{
	// Tell engine to use UCI
	engine_process->host_to_engine_ << "uci\n"
									<< std::flush;
	// Wait for uciok reply
	auto replies = read_uci_replies(engine_process->engine_to_host_);
	if (replies.empty())
		throw std::runtime_error("Engine error: Engine did not send the 'uciok' message");
	if (replies.back() != "uciok")
		throw std::runtime_error("Engine error: Unexpected engine message. Expected 'uciok', got " + replies.back() + ".");
	// Set multi pv setting
	engine_process->host_to_engine_ << "setoption name MultiPV value " << (int)num_best_lines << "\n"
									<< std::flush;
	// Set max ELO rating
	if (max_elo_rating.has_value()) {
		engine_process->host_to_engine_ << "setoption name UCI_LimitStrength value true\n"
										<< std::flush;
		engine_process->host_to_engine_ << "setoption name UCI_Elo value " << *max_elo_rating << "\n"
										<< std::flush;
	}

	// Send isready command and wait for reply
	engine_process->host_to_engine_ << "isready\n"
									<< std::flush;
	replies = read_isready_replies(engine_process->engine_to_host_);
	if (replies.empty())
		throw std::runtime_error("Engine error: Engine did not send the 'isready' message");
	if (replies.back() != "readyok")
		throw std::runtime_error("Engine error: Unexpected engine message. Expected 'readyok', got " + replies.back() + ".");
}

Engine ::~Engine()
{}

void Engine::reset_game()
{
	// Stop any running calculation
	stop_calculating();
	// Stored game continuations are no longer valid
	suggested_lines_.clear();
	// Reset game state in engine
	engine_process->host_to_engine_ << "ucinewgame\n"
									<< std::flush;
	engine_process->host_to_engine_ << "position startpos\n"
									<< std::flush;

	// Send isready command and wait for reply
	engine_process->host_to_engine_ << "isready\n"
									<< std::flush;
	auto replies = read_isready_replies(engine_process->engine_to_host_);
	if (replies.empty())
		throw std::runtime_error("Engine error: Engine did not send the 'readyok' message");
	if (replies.back() != "readyok")
		throw std::runtime_error("Engine error: Unexpected engine message. Expected 'readyok', got " + replies.back() + ".");
}

void Engine::setup_game_from_fen(const std::string& fen)
{
	// Stop any running calculation
	stop_calculating();
	// Stored game continuations are no longer valid
	suggested_lines_.clear();
	// Update game state in engine
	engine_process->host_to_engine_ << "ucinewgame\n"
									<< std::flush;
	engine_process->host_to_engine_ << "position fen " << fen << "\n"
									<< std::flush;

	// Send isready command and wait for reply
	engine_process->host_to_engine_ << "isready\n"
									<< std::flush;
	auto replies = read_isready_replies(engine_process->engine_to_host_);
	if (replies.empty())
		throw std::runtime_error("Engine error: Engine did not send the 'readyok' message");
	if (replies.back() != "readyok")
		throw std::runtime_error("Engine error: Unexpected engine message. Expected 'readyok', got " + replies.back() + ".");
}

void Engine::setup_game_from_moves(const std::string& lan)
{
	// Stop any running calculation
	stop_calculating();
	// Stored game continuations are no longer valid
	suggested_lines_.clear();
	// Update game state in engine
	engine_process->host_to_engine_ << "ucinewgame\n"
									<< std::flush;
	engine_process->host_to_engine_ << "position startpos moves " << lan << "\n"
									<< std::flush;

	// Send isready command and wait for reply
	engine_process->host_to_engine_ << "isready\n"
									<< std::flush;
	auto replies = read_isready_replies(engine_process->engine_to_host_);
	if (replies.empty())
		throw std::runtime_error("Engine error: Engine did not send the 'readyok' message");
	if (replies.back() != "readyok")
		throw std::runtime_error("Engine error: Unexpected engine message. Expected 'readyok', got " + replies.back() + ".");
}

void Engine::start_calculating(
	std::optional<std::chrono::seconds> max_calculation_time)
{
	// Stop any running calculation
	stop_calculating();

	// Send go command to engine
	engine_process->host_to_engine_ << "go ";
	if (max_calculation_time.has_value()) {
		engine_process->host_to_engine_ << "movetime " << std::chrono::milliseconds(*max_calculation_time).count() << "\n"
										<< std::flush;
	} else {
		engine_process->host_to_engine_ << "infinite\n"
										<< std::flush;
	}

	// Keep track of the fact that we have started a calculation (whose output we need to manage before any other calculation can be started)
	is_calculating_ = true;
}

void Engine::stop_calculating()
{
	if (!is_calculating_)
		// No calculation started
		return;

	// Send stop calculating command to the engine
	engine_process->host_to_engine_ << "stop\n"
									<< std::flush;
	// Read go replies
	auto messages = read_go_replies(engine_process->engine_to_host_);
	// Parse engine messages
	parse_engine_go_messages(messages);
	// Set is calculating flag
	is_calculating_ = false;
}

Evaluation Engine::get_evaluation() const
{
	if (suggested_lines_.empty())
		throw std::runtime_error("Evaluation called failed: No engine results available. Need to run start_calculating and stop_calculating first");

	return suggested_lines_.front().evaluation;
}

const std::vector<Analyzed_line>& Engine::get_top_suggested_move_sequences() const
{
	return suggested_lines_;
}

void Engine::parse_engine_go_messages(const std::vector<std::string>& messages)
{
	suggested_lines_.resize(num_best_lines_);

	assert(messages.size() >= num_best_lines_ + 1);

	// Only read the last num_best_lines_ info messages
	size_t idx = messages.size() - 1;
	size_t info_messages_read = 0;
	while (info_messages_read < num_best_lines_) {
		if (messages[idx].substr(0, 4) == "info") {
			Info info = parse_info(messages[idx]);

			assert(info.evaluation.has_value());
			assert(info.line_index.has_value());
			assert(info.sequence_of_moves.has_value());

			suggested_lines_[*info.line_index].evaluation = *info.evaluation;
			suggested_lines_[*info.line_index].moves = std::move(*info.sequence_of_moves);
			info_messages_read += 1;
		}
		idx -= 1;
	}
}

} // namespace uci
} // namespace chess
