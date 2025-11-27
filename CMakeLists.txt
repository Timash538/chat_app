cmake_minimum_required(VERSION 3.10)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
  message(STATUS "Bootstrapping vcpkg...")
  execute_process(
    COMMAND git clone https://github.com/microsoft/vcpkg.git
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
endif()

set(CMAKE_TOOLCHAIN_FILE "vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Path to vcpkg toolchain file")

project(chat_app)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(asio CONFIG REQUIRED)

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
add_subdirectory(tests)

