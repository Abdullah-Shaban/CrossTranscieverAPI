include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(eshter_rx_cfile CFileRX.cpp)
target_link_libraries(eshter_rx_cfile XCVR_Eshter)

add_executable(eshter_xcvr_demo XCVRDemo.cpp)
target_link_libraries(eshter_xcvr_demo XCVR_Eshter)
