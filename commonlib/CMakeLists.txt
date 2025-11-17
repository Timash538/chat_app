add_library(commonlib
	src/User.cpp
	src/Message.cpp
	src/Socket.cpp
)

target_include_directories(commonlib PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

