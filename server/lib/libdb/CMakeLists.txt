cmake_minimum_required(VERSION 3.20)
project(dbrequests)

set(SOURCES src/db.cpp)
set(HEADERS include/config.h include/db.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC mysqlcppconn)






