add_executable(client
)

target_include_directories(client PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(client PRIVATE commonlib)

file(GLOB_RECURSE HEADERS
    LIST_DIRECTORIES false
    include/*.h
)

file(GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
    src/*.cpp
)

target_sources(client PRIVATE ${SOURCES})

source_group("Header files" FILES ${HEADERS})
source_group("Source files" FILES ${SOURCES})

if(WIN32)
    target_link_libraries(client PRIVATE ws2_32)
endif()