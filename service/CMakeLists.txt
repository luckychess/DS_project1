cmake_minimum_required(VERSION 3.2.2)
project(simple_chat_service)

set (CMAKE_CXX_STANDARD 11)
set (SOURCES message.cpp)

find_package(Boost REQUIRED system)
if(NOT Boost_FOUND)
	message(SEND_ERROR "Failed to find boost.")
	return()
else()
	include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(service STATIC ${SOURCES} ${Boost_LIBRARIES})
