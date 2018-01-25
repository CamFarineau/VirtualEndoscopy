cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(VirtualEndoscopy)

set(CMAKE_PREFIX_PATH /Path/To/VTK/)
set(QT_QMAKE_EXECUTABLE /Path/To/qmake)
set(VTK_DIR /Path/To/VTK/Build/)

find_package(VTK COMPONENTS
  vtkGUISupportQt
  vtkIOImage
  vtkInteractionImage
  vtkRenderingOpenGL2
  vtkTestingCore
)
include(${VTK_USE_FILE})

if("${VTK_QT_VERSION}" STREQUAL "")
  message(FATAL_ERROR "VTK was not built with Qt")
endif()

file(GLOB DICOMReader_SRC
    "src/*.h"
    "src/*.cxx"
)
find_package(Qt5 COMPONENTS Core REQUIRED QUIET)

add_executable(VirtualEndoscopy ${DICOMReader_SRC})
qt5_use_modules(VirtualEndoscopy Core Gui Widgets)
target_link_libraries(VirtualEndoscopy ${VTK_LIBRARIES})
