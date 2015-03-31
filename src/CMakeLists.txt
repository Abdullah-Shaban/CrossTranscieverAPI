include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(XCVR_Eshter SHARED
	SpectrumSensor.cpp
	DeviceImp.cpp
	DeviceController.cpp
	Scheduler.cpp
)
target_link_libraries(XCVR_Eshter ${serial_LIBRARIES} ${Boost_LIBRARIES})
