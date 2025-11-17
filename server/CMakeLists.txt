add_executable(server 
	src/main.cpp 
 "include/Server.h" "src/Server.cpp")

target_include_directories(server PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE commonlib)

# Платформенные зависимости
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
endif()