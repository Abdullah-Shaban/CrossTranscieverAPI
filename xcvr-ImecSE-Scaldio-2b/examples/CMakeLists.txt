cmake_minimum_required(VERSION 2.8.7)

include_directories(${PROJECT_SOURCE_DIR}/include) 

find_package(Boost REQUIRED
	COMPONENTS thread system chrono
)

# find_package(serial REQUIRED)

enable_language(CXX)
add_definitions ("-Wall -g -std=c++11")

#add_executable( test  scaldio_single_ADC_LOG1.c )
#add_executable( test2  test2.cpp )
add_executable( imec-se-test imec-se-test.cpp )
target_link_libraries(imec-se-test XCVR_imec-se-scaldio-2b sensing ssi spider usb m)
#target_link_libraries(test sensing ssi usb m)
#target_link_libraries(test2 XCVR_imec-se-scaldio-2b sensing ssi spider usb m)



