cmake_minimum_required(VERSION 3.20)
# set(Boost_DEBUG ON)
project(client VERSION 0.0.1)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_DEBUG_LIBS        OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

set(HEADERS include/client.hpp include/message.hpp)
set(SOURCES src/main.cpp src/client.cpp)

find_package(Boost 1.7 REQUIRED COMPONENTS system serialization date_time)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS} include)
  add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
  message("Boost version: " ${Boost_VERSION})
endif()

if(WIN32)
  target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION client/bin)
