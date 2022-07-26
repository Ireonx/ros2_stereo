cmake_minimum_required(VERSION 3.5)
project(camera_controller)

add_compile_options(-std=c++11)

cmake_policy(SET CMP0057 NEW)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(camera_calibration_parsers REQUIRED)
find_package(camera_info_manager REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)
find_package(OpenCV 3 REQUIRED)
find_package(libtlive REQUIRED)
find_package(libtconfig REQUIRED)

ament_export_dependencies(rclcpp visualization_msgs robot_model_helper compute_cdist)

add_executable(node
        src/main.cpp
)


ament_target_dependencies(node sensor_msgs  rclcpp std_msgs cv_bridge camera_calibration_parsers)

target_include_directories(node PUBLIC
        ${OpenCV_INCLUDE_DIRS}
        ${LIBTLIVE_INCLUDE_DIRS}
        ${LIBTCONFIG_INCLUDE_DIRS}
)

target_link_libraries(node
        ${OpenCV_LIBRARIES}
        ${LIBTLIVE_LIBRARIES}
	      ${LIBTCONFIG_LIBRARIES}
)

install(TARGETS
  node
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
