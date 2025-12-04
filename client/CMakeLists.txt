add_executable(client
)

target_include_directories(client PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(client PRIVATE common)

file(GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
    src/*.cpp
)

target_sources(client PRIVATE ${SOURCES})

if(WIN32)
    target_link_libraries(client PRIVATE ws2_32)
endif()