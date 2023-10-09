cmake_minimum_required(VERSION 3.2)

project(server VERSION 0.0.1)

find_package(Boost REQUIRED COMPONENTS system)
find_package(MySQLConnectorCPP)

add_subdirectory(lib/libdb)

set(SOURCES src/server.cpp)

add_executable(server ${SOURCES})

target_include_directories(server PUBLIC 
	${PROJECT_BINARY_DIR}
) 

target_link_libraries(server PUBLIC dbrequests Boost::system mysqlcppconn)
