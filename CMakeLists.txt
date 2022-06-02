cmake_minimum_required(VERSION 3.1.9)


project(matlabapi)
# As of R2021a, C++ version ~11 and 14 are supported
set(CMAKE_CXX_STANDARD 14)
# To avoid "undefined reference to symbol 'pthread_create@@GLIBC_x.x.x" error
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") 
# Set the matlab root directory
set(Matlab_ROOT_DIR  /usr/local/MATLAB/R2022a)
# Set LD_LIBRARY_PATH for execution
set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${Matlab_ROOT_DIR}/extern/bin/glnxa64:${Matlab_ROOT_DIR}/sys/os/glnxa64)
# Included directories
include_directories(${Matlab_ROOT_DIR}/extern/include/)
# Print all included directories
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
        message(STATUS "Included Directory is: '${dir}'")
endforeach()
# Add linker search directories
link_directories(${Matlab_ROOT_DIR}/extern/bin/glnxa64)
# Executable
add_executable(matlabapi numbers.cpp)
# Linker stuff
# Linker to libMatlabDataArray in link_directories
target_link_libraries(matlabapi MatlabDataArray)
# Linker to libMatlabEngine in link_directories
target_link_libraries(matlabapi MatlabEngine)