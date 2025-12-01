add_executable(server 
	src/main.cpp 
    src/Server.cpp
)

target_include_directories(server PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE commonlib asio::asio)

file(GLOB_RECURSE HEADERS
    LIST_DIRECTORIES false
    include/*.h
)

file(GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
    src/*.cpp
)

target_sources(server PRIVATE ${SOURCES})

source_group("Header files" FILES ${HEADERS})
source_group("Source files" FILES ${SOURCES})

# Платформенные зависимости
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
endif()