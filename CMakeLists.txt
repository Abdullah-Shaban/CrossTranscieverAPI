cmake_minimum_required(VERSION 2.8.7)
project (xcvr-eshter)

find_package(Boost
	COMPONENTS regex
)

enable_language(CXX)
add_definitions ("-Wall")

include_directories("/home/avian/patch/serial/include")
link_directories("/home/avian/patch/serial/build")

add_subdirectory(src)
add_subdirectory(tests)
