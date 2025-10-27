cmake_minimum_required(VERSION 3.10)
project(OTA_Gateway)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -Werror -Wpedantic")
include_directories(Inc)
include_directories(utility)
add_executable(OTA_Gateway Src/main.cpp Src/Server.cpp)
add_executable(client_test_raspberry_pi Src/client_test_raspberry_pi.cpp)
add_executable(client_test_local Src/client_test_local.cpp)

target_link_libraries(OTA_Gateway PRIVATE Threads::Threads)

