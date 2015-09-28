cmake_minimum_required(VERSION 2.8.7)

include_directories(${PROJECT_SOURCE_DIR}/sensing/include ${PROJECT_SOURCE_DIR}/include) 
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/sensing/lib)

find_package(Boost REQUIRED
	COMPONENTS thread system chrono
)

# find_package(serial REQUIRED)

enable_language(CXX)
add_definitions ("-Wall -g -std=c++11")

add_executable( imec-se-test imec-se-test.cpp )
target_link_libraries(imec-se-test XCVR_imec-se-warp sensing swi usb m)



