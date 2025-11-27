add_executable(tests
	main.cpp 
    InterruptableThread.cpp
	ThreadPool.cpp
)

target_include_directories(tests PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Платформенные зависимости
if(WIN32)
    target_link_libraries(tests PRIVATE ws2_32)
endif()