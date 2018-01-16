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

set( Srcs DICOMImageReaderFileCollection.cxx KeyPressInteractorStyle.cxx vtkImageInteractionCallback.cxx)
set( Hdrs KeyPressInteractorStyle.h vtkImageInteractionCallback.h)
find_package(Qt5 COMPONENTS Core REQUIRED QUIET)

add_executable(ImageReaderDicomCollection ${Srcs} ${Hdrs})
qt5_use_modules(ImageReaderDicomCollection Core Gui Widgets)
target_link_libraries(ImageReaderDicomCollection ${VTK_LIBRARIES})
