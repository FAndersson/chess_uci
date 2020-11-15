C++ Chess UCI Interface
=======================

Introduction and motivation
---------------------------

This library provides a C++ interface for interacting with a UCI (
[Universal Chess Interface](https://en.wikipedia.org/wiki/Universal_Chess_Interface))
conforming chess engine.  
The UCI interface specifies how a host process can communicate with a child
process running a chess engine by means of text messages to/from the standard
input/output. The benefit of this model is that it's quite general, for example
enabling the host process and the chess engine to be implemented in different
programming languages. However setting up and managing the communication with
the child engine process can be a bit awkward. So this library provides a C++
interface for handling this, making interaction with a chess engine as easy as
creating an instance of chess engine interface class, and calling methods on
this object.

Usage
-----

For details about the engine interface class see
[chess_uci/Engine.h](chess_uci/Engine.h). The library is built using CMake, see
the [CMakeLists.txt](CMakeLists.txt) file for details. An example for how to
build the library and use it to analyze a chess game is given in
[examples/Analyze_carlsen_caruana_example.cpp](examples/Analyze_carlsen_caruana_example.cpp).
