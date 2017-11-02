cmake_minimum_required(VERSION 3.8)
project(Project_3)


find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(Project_3 ${SOURCE_FILES})
target_link_libraries(Project_3 ${OpenCV_LIBS} )