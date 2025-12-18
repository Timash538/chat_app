find_package(GTest REQUIRED
)

add_executable(tests
		User_test.cpp
		Message_test.cpp
)

target_link_libraries(tests
	PRIVATE
		GTest::gtest_main
		common
)

add_test(NAME AllTests COMMAND tests)