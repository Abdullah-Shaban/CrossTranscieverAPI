include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(AllTests AllTests.cpp)
target_link_libraries(AllTests CppUTest XCVR_Eshter)

add_custom_target(
	test ALL
	COMMAND AllTests
)
