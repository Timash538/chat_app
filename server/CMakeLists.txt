add_executable(server 
	src/main.cpp 
    src/Server.cpp
	src/ChatManager.cpp
	src/UserManager.cpp
 "include/thread_pool/ThreadPool.h" "src/thread_pool/ThreadPool.cpp" "include/thread_pool/BlockedQueue.h" "src/thread_pool/InterruptableThread.cpp" "include/thread_pool/InterruptableThread.h")



target_include_directories(server PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/thread_pool
)

target_link_libraries(server PRIVATE commonlib asio::asio)

# Платформенные зависимости
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
endif()