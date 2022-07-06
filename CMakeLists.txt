cmake_minimum_required(VERSION 3.1.9)

 
project(main)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

if (WIN32)
        set("OpenCV_DIR" "C:/Program Files (x86)/opencv/build/x64/vc15/lib") 
endif (WIN32)
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
add_executable(main src/Field.cpp src/Agent.cpp src/SignalSource.cpp src/main.cpp src/Ring.cpp)
target_link_libraries(main ${OpenCV_LIBS})
