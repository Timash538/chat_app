add_executable(client
	src/main.cpp
	src/MenuController.cpp
	src/Client.cpp
   "include/IChatService.h" "include/UserService.h" "include/ChatService.h" "include/IUserService.h")

target_include_directories(client PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(client PRIVATE commonlib)

if(WIN32)
    target_link_libraries(client PRIVATE ws2_32)
endif()