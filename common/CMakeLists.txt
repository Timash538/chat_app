add_library(common
)

target_link_libraries(common PUBLIC nlohmann_json::nlohmann_json)

target_include_directories(common 
    PUBLIC 
	    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
    src/*.cpp
)

target_sources(common PRIVATE ${SOURCES})

source_group("Source files" FILES ${SOURCES})
