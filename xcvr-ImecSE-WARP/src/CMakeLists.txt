cmake_minimum_required(VERSION 2.8.7)

include_directories(${PROJECT_SOURCE_DIR}/sensing/include ${PROJECT_SOURCE_DIR}/include)

add_library(XCVR_imec-se-warp SHARED
	ImecSE.cpp
)

target_link_libraries(XCVR_imec-se-warp)