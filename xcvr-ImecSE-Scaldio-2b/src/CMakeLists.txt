cmake_minimum_required(VERSION 2.8.7)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(XCVR_imec-se-scaldio-2b SHARED
	ImecSE-scaldio-2b.cpp
)

target_link_libraries(XCVR_imec-se-scaldio-2b)