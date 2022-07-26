find_package(OpenCV 3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_library(${PROJECT_OUTPUT_LIB} SHARED 	
	tlive_stream_client.cpp
	tlive_client.cpp)

target_link_libraries(${PROJECT_OUTPUT_LIB} tlive-lz4 tlive-zstd tlive-3dproc ${TBB_LIBRARIES} ${ZeroMQ_LIBRARY} ${OpenCV_LIBRARIES})
install(TARGETS ${PROJECT_OUTPUT_LIB} EXPORT libtlive-targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static COMPONENT library)
