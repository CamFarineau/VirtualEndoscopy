cmake_minimum_required(VERSION 2.8)

PROJECT(VirtualEndoscopy)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(VirtualEndoscopy ./src/VirtualEndoscopy.cpp )

target_link_libraries(VirtualEndoscopy ${VTK_LIBRARIES})
