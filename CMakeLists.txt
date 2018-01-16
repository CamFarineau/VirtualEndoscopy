cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ImageReaderDicomCollection)

set(CMAKE_PREFIX_PATH /home/camille/VTK/)
set(QT_QMAKE_EXECUTABLE /usr/lib/x86_64-linux-gnu/qt5/bin/qmake)
set(VTK_DIR /home/camille/VTK/Build/)

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

add_executable(ImageReaderDicomCollection ${DICOMReader_SRC})
qt5_use_modules(ImageReaderDicomCollection Core Gui Widgets)
target_link_libraries(ImageReaderDicomCollection ${VTK_LIBRARIES})
