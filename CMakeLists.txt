cmake_minimum_required(VERSION 2.8.7)
project (xcvr-eshter)

enable_language(CXX)

include_directories("/home/avian/src/serial/include")
link_directories("/home/avian/src/serial/build")

add_subdirectory(src)
add_subdirectory(tests)
