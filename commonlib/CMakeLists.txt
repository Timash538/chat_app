add_library(commonlib
)

target_link_libraries(commonlib PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(commonlib 
    PUBLIC 
	    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCES
    LIST_DIRECTORIES false
    src/*.cpp
)

target_sources(commonlib PRIVATE ${SOURCES})

source_group("Source files" FILES ${SOURCES})
