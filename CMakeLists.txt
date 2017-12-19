cmake_minimum_required(VERSION 2.8)

PROJECT(ImageRender)

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
set(QT_USE_OPENGL TRUE)
INCLUDE(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})


set(CMAKE_PREFIX_PATH /home/nico/Documents/5A/Projet_Majeure/VTK/build/)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS( -Wall -Wextra -std=c++11 -Wno-comment -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -DCORRECTION)


add_executable(ImageRender MACOSX_BUNDLE main.cpp )

target_link_libraries(ImageRender ${VTK_LIBRARIES})
TARGET_LINK_LIBRARIES(ImageRender -lm -ldl -lGLEW ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${QT_GL_LIBRARIES} ${QT_QTOPENGL_LIBRARY})

