cmake_minimum_required(VERSION 2.8.7)
project (xcvr-imec-se)

include_directories(${PROJECT_SOURCE_DIR}/include) 

find_package(Boost REQUIRED
	COMPONENTS thread system chrono
)

# find_package(serial REQUIRED)

enable_language(CXX)
add_definitions ("-Wall -g -std=c++11")


add_subdirectory(src)
add_subdirectory(examples)

