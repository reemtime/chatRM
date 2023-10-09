cmake_minimum_required(VERSION 3.2)

project(client VERSION 0.0.1)

find_package(Boost REQUIRED COMPONENTS system)
set(SOURCES src/client.cpp)


add_executable(client ${SOURCES})

target_link_libraries(client PUBLIC Boost::system)
