#include <iostream>

int main()
{
	while (true) {
		std::string command;
		std::getline(std::cin, command);
		if (command == "uci") {
			std::cout << "uciok\n";
		}
		if (command == "isready") {
			std::cout << "readyok\n";
		}
		if (command == "stop")
			std::cout << "bestmove e2e4\n";
		if (command == "quit")
			return 0;
		if (command == "go")
			// Do nothing
			;
		if (command == "ucinewgame")
			// Do nothing
			;
		if (command == "position startpos")
			// Do nothing
			;
	}
}