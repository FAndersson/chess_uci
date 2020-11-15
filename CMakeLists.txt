cmake_minimum_required(VERSION 3.10)

project(cpp_chess_uci)

set(CMAKE_CXX_STANDARD 17)

find_package(Catch2 REQUIRED)

add_library(uci_support
	chess_uci/Evaluation.cpp
	chess_uci/Parse_messages.cpp
	chess_uci/Read_messages.cpp)

add_library(uci_engine chess_uci/Engine.cpp)

add_executable(dummy_engine chess_uci/test/Dummy_engine.cpp)

add_executable(engine_communication_test chess_uci/test/Engine_communication_test.cpp)
target_include_directories(engine_communication_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(engine_communication_test boost_iostreams boost_system boost_thread pthread Catch2::Catch2)

add_executable(engine_test chess_uci/test/Engine_test.cpp)
target_include_directories(engine_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(engine_test boost_iostreams boost_system boost_thread pthread uci_engine uci_support Catch2::Catch2)

add_executable(evaluation_test chess_uci/test/Evaluation_test.cpp)
target_include_directories(evaluation_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(evaluation_test uci_support Catch2::Catch2)

add_executable(parse_messages_test chess_uci/test/Parse_messages_test.cpp)
target_include_directories(parse_messages_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(parse_messages_test uci_support Catch2::Catch2)

add_executable(read_messages_test chess_uci/test/Read_messages_test.cpp)
target_include_directories(read_messages_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(read_messages_test uci_support Catch2::Catch2)

enable_testing()
add_test(NAME engine_communication_test COMMAND engine_communication_test)
add_test(NAME engine_test COMMAND engine_test)
add_test(NAME evaluation_test COMMAND evaluation_test)
add_test(NAME parse_messages_test COMMAND parse_messages_test)
add_test(NAME read_messages_test COMMAND read_messages_test)
