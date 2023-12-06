cmake_minimum_required(VERSION 3.2)

project(server VERSION 0.0.1)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_DEBUG_LIBS        OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

set(HEADERS include/message.hpp include/tcp_connection.hpp include/tcp_server.hpp)
set(SOURCES src/server.cpp src/tcp_connection.cpp src/tcp_server.cpp)

find_package(Boost  1.70 REQUIRED COMPONENTS system serialization)

add_subdirectory(lib/libdb)

if(Boost_FOUND)
  add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
  target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR} ${Boost_INCLUDE_DIRS} include)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES} dbrequests)
  message("Boost version: " ${Boost_VERSION})
endif()

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32 wsock32)
endif()

install(TARGETS server DESTINATION server/bin)
