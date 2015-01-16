include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(OffLineTests Main.cpp SpectrumSensorTests.cpp DeviceImpTests.cpp)
target_link_libraries(OffLineTests CppUTest XCVR_Eshter)

add_executable(OnLineTests Main.cpp OnLineTests.cpp)
target_link_libraries(OnLineTests CppUTest XCVR_Eshter)

add_custom_target(
	test ALL
	COMMAND OffLineTests
)

add_custom_target(
	test_online
	COMMAND OnLineTests
)
