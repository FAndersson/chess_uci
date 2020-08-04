cmake_minimum_required(VERSION 3.10)

project(cpp_chess_uci)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_library(uci_support Evaluation.cpp Parse_messages.cpp Read_messages.cpp)

add_executable(dummy_engine test/Dummy_engine.cpp)
add_executable(engine_communication_test test/Engine_communication_test.cpp)
target_link_libraries(engine_communication_test boost_iostreams boost_system boost_thread pthread)

add_executable(evaluation_test test/Evaluation_test.cpp)
target_link_libraries(evaluation_test uci_support)

add_executable(parse_messages_test test/Parse_messages_test.cpp)
target_link_libraries(parse_messages_test uci_support)

add_executable(read_messages_test test/Read_messages_test.cpp)
target_link_libraries(read_messages_test uci_support)

enable_testing()
add_test(NAME engine_communication_test COMMAND engine_communication_test)
add_test(NAME evaluation_test COMMAND evaluation_test)
add_test(NAME parse_messages_test COMMAND parse_messages_test)
add_test(NAME read_messages_test COMMAND read_messages_test)
