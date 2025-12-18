add_executable(server 
)

target_include_directories(server PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE common asio::asio PostgreSQL::PostgreSQL unofficial-sodium::sodium)

file(GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
    src/*.cpp
)

target_sources(server PRIVATE ${SOURCES})

# Платформенные зависимости
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
endif()