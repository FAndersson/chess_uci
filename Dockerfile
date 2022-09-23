# Docker image containing tools for development of C++ chess tools

# Base on general image for C++ development. Using testing version for full
# GCC support of std::filesystem.
FROM fredrikandersson/debian-testing-cpp-image-gcc

RUN apt-get update --quiet --yes

# Install Stockfish chess engine (for testing)
RUN apt-get install --quiet --yes stockfish

# Install boost packages used by the chess engine interface
RUN apt-get install --quiet --yes libboost-dev libboost-iostreams-dev libboost-system-dev libboost-thread-dev

# Install Catch2 required for the chess engine interface tests
RUN apt-get install --quiet --yes catch2

