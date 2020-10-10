/**
 * \file Engine.h
 * \brief Contains an engine interface class used to run and communicate with a UCI chess engine.
 */

#pragma once 

#include "Evaluation.h"
#include "Line.h"

#include <boost/process.hpp>

#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace chess {
namespace uci {
	
/**
 * \class Engine
 * \brief Interface to run and communicate with a backend UCI (universal chess interface) compatible
 * chess engine, specified as an executable.
 *
 * Usage:
 * 1. Create Engine object with path to the chess engine to run. This will start the engine in a child process.
 * 2. Setup game using reset_game() (to start from the beginning) or setup_game() (to start from a specific position).
 * 3. Call start_calculating() to tell the engine to start calculating, and then (at some later time) call
 *    stop_calculating() to stop the engine calculation and process the engine output.
 * 4. Call evaluation() or get_top_suggested_move_sequences() to get output from the engine calculation.
 * 5. Repeat 2-4 at will.
 * 6. Destroy the Engine object. This will stop the engine child process.
 */
class Engine
{
public:
	/**
     * \param engine_executable Path to the engine executable.
     * Executable will be started in a subprocess and shut down
     * when the Engine object is destroyed.
     * \param num_best_lines Number of best lines to suggest.
     * \param max_elo_rating Max ELO rating the engine is allowed to play at. If not specified there is no such limit.
     */
	Engine(const std::filesystem::path& engine_executable, uint8_t num_best_lines = 1, std::optional<uint16_t> max_elo_rating = std::nullopt);
	~Engine();

	/// Reset the chess game to the starting position.
	void reset_game();
	/**
     * Set game to the given state.
     *
     * \param fen Chess game state specified as a
     * Forsyth-Edwards Notation (FEN) string.
     */
	void setup_game(const std::string& fen);

	/**
     * Start calculating from the current position.
     *
     * Note: Should not be called while the engine is already calculating
     * (as checked by is_calculating()). If a new calculation should be
     * started with other settings than the currently running calculation
     * then stop_calculating() should be called first.
     *
     * \param max_calculation_time Maximum time (in seconds) the engine is
     * allowed to calculate. If not specified there is no limit, and the engine
     * will calculate until stop_calculating is called.
     */
	void start_calculating(std::optional<std::chrono::seconds> max_calculation_time = std::nullopt);
	/// Tell the engine to stop calculating.
	void stop_calculating();

	/**
     * Get evaluation of the current game position.
     * Should be called after the engine has been calculating on the
     * current position (see start_calculating()).
     *
     * \return Evaluation of the current game position.
     */
	Evaluation get_evaluation() const;

	/**
     * Get the top move sequences (lines) from the current position.
     * Should be called after the engine has been calculating on the
     * current position (see start_calculating()).
     *
     * \return Array of top suggested lines. The first entry in the
     * array is the top suggestion, the second entry is the second best
     * suggestion and so on.
     */
	const std::vector<Analyzed_line>& get_top_suggested_move_sequences() const;

private:
    /// Parse engine messages received after the previous go command.
	void parse_engine_go_messages(const std::vector<std::string>& messages);

    /// Stream which the engine writes it's messages to.
	boost::process::pstream engine_to_host_;
    /// Stream which the engine reads commands from.
	boost::process::pstream host_to_engine_;

    /// Child process running the engine.
	boost::process::child engine_child_process_;

    /// Number of best lines the engine should calculate.
    uint8_t num_best_lines_{1};
    /// Whether or not we have a started engine calculation, whose output has not yet been processed.
    bool is_calculating_{false};
    /// Top suggested lines from the last engine calculation.
    std::vector<Analyzed_line> suggested_lines_;
};

} // namespace uci
} // namespace chess
