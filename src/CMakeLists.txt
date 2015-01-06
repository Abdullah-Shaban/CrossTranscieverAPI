include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(XCVR_Eshter
	SpectrumSensor.cpp
	DeviceImp.cpp
)

target_link_libraries(XCVR_Eshter serial ${Boost_LIBRARIES})
