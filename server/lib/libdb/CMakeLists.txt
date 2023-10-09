cmake_minimum_required(VERSION 3.2)

project(dbrequests)

set(SOURCES src/db.cpp)
set(HEADERS include/config.h include/db.h)

add_library(dbrequests ${SOURCES} ${HEADERS})
target_include_directories(dbrequests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)





