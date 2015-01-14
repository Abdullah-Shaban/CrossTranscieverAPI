cmake_minimum_required(VERSION 2.8.7)
project (xcvr-eshter)

find_package(Boost REQUIRED
	COMPONENTS regex
)

find_package(serial REQUIRED)

enable_language(CXX)
add_definitions ("-Wall")

include_directories(${serial_INCLUDE_DIRS})
link_directories(${serial_LIBRARY_DIRS})

add_subdirectory(src)
add_subdirectory(tests)
