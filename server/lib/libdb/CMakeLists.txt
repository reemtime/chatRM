cmake_minimum_required(VERSION 3.20)

if(WIN32)
  set(mysqlconn_include "C:/Program Files/MySQL/mysql-connector-c++-8.2.0-winx64/include/jdbc")
  set(mysqlcppconn_lib "C:/Program Files/MySQL/mysql-connector-c++-8.2.0-winx64/lib64/vs14/mysqlcppconn.lib")
endif()


project(dbrequests)

set(SOURCES src/db.cpp)
set(HEADERS include/config.h include/db.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(UNIX)
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${PROJECT_NAME} PUBLIC mysqlcppconn)
endif()

if(WIN32)
  target_include_directories(${PROJECT_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${mysqlconn_include})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${mysqlcppconn_lib})
endif()



