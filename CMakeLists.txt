cmake_minimum_required(VERSION 3.1.9)

 
project(main)
# As of R2021a, C++ version ~11 and 14 are supported
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

# set("OpenCV_DIR" "/usr/share/OpenCV") 
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ext_libs )

add_definitions(-DOPENCV_DISABLE_EIGEN_TENSOR_SUPPORT)

MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")

# Print all included directories
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
        message(STATUS "Included Directory is: '${dir}'")
endforeach()

# Executable
add_executable(main src/Field.cpp src/Agent.cpp src/SignalSource.cpp src/main.cpp)
target_link_libraries(main ${OpenCV_LIBS})
