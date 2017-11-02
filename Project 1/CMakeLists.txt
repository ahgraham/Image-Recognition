cmake_minimum_required(VERSION 3.8)
project(Project_1)


find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(Project_1 ${SOURCE_FILES})
target_link_libraries(Project_1 ${OpenCV_LIBS} )