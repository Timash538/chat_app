cmake_minimum_required(VERSION 3.10)
project(chat_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	add_definitions(-D_WIN32_WINT=0x0601)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_subdirectory(commonlib)
add_subdirectory(server)
add_subdirectory(client)