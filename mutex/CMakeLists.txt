cmake_minimum_required(VERSION 3.16)

### --------------------------------------------------------------------
### Project information
### --------------------------------------------------------------------
set(PROJECT_NAME mutex)
project(${PROJECT_NAME} CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
### --------------------------------------------------------------------
### Set arm platform macro
### --------------------------------------------------------------------

set(BUILD_PLATFORM arm)

include_directories(
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_BINARY_DIR}/
    /usr/include/opencv4
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib/
)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-Wall -Wno-class-memaccess)

### --------------------------------------------------------------------
### Import modules
### --------------------------------------------------------------------
add_subdirectory(${CMAKE_SOURCE_DIR}/modules/)

