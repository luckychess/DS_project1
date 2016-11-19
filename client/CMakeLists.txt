cmake_minimum_required(VERSION 3.2.2)
project(simple_chat_client)

set (CMAKE_CXX_STANDARD 11)
set (SOURCES client.cpp)

include_directories(../service)
add_executable(client ${SOURCES})

find_package(Boost REQUIRED system)
if(NOT Boost_FOUND)
	message(SEND_ERROR "Failed to find boost.")
	return()
else()
	include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(client service ${Boost_LIBRARIES} Threads::Threads)