cmake_minimum_required(VERSION 3.5)
project(opencl_scan)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -Wno-deprecated-declarations -Wno-ignored-attributes -Wall -Werror")
set(BINARY_NAME ${PROJECT_NAME})

find_package(OpenCL REQUIRED)

add_executable(${BINARY_NAME} src/main.cpp)
target_link_libraries(${BINARY_NAME} ${OpenCL_LIBRARIES})
target_include_directories(${BINARY_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
target_include_directories(${BINARY_NAME} PRIVATE include)

configure_file(src/scan.cl ${CMAKE_BINARY_DIR}/scan.cl COPYONLY)
configure_file(include/config.h ${CMAKE_BINARY_DIR}/config.h COPYONLY)

enable_testing()
add_subdirectory(test)
