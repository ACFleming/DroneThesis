cmake_minimum_required(VERSION 3.1.9)


project(multiAgent)
# As of R2021a, C++ version ~11 and 14 are supported
set(CMAKE_CXX_STANDARD 14)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

# Print all included directories
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
        message(STATUS "Included Directory is: '${dir}'")
endforeach()

# Executable
add_executable(multiAgent src/Field.cpp src/Agent.cpp src/SignalSource.cpp src/main.cpp)
