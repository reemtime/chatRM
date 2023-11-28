cmake_minimum_required(VERSION 3.2)

project(server VERSION 0.0.1)

find_package(Boost  1.70 REQUIRED COMPONENTS system serialization)

add_subdirectory(lib/libdb)

set(HEADERS include/message.hpp include/tcp_connection.hpp include/tcp_server.hpp)

set(SOURCES src/server.cpp src/tcp_connection.cpp src/tcp_server.cpp)

add_executable(server ${SOURCES} ${HEADERS})

target_include_directories(server PUBLIC 
        ${PROJECT_BINARY_DIR} include
) 

target_link_libraries(server PUBLIC ${Boost_LIBRARIES} dbrequests mysqlcppconn)

install(TARGETS server DESTINATION server/bin)
