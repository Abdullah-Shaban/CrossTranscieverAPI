include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(XCVR_Eshter
	SpectrumSensor.cpp
	DeviceImp.cpp
	DeviceController.cpp
	Scheduler.cpp
)
target_link_libraries(XCVR_Eshter ${serial_LIBRARIES} ${Boost_LIBRARIES})

add_executable(eshter_rx_cfile CFileRX.cpp)
target_link_libraries(eshter_rx_cfile XCVR_Eshter)
