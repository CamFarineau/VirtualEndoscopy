cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ImageReaderDicomCollection)

set(CMAKE_PREFIX_PATH /home/nico/Documents/5A/Projet_Majeure/VTK/)
set(QT_QMAKE_EXECUTABLE /usr/lib/x86_64-linux-gnu/qt4/bin/qmake)
set(VTK_DIR /home/nico/Documents/5A/Projet_Majeure/VTK/build/)

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

set( Srcs DICOMImageReaderFileCollection.cxx )

find_package(Qt5 COMPONENTS Core REQUIRED QUIET)

add_executable(ImageReaderDicomCollection ${Srcs})
qt5_use_modules(ImageReaderDicomCollection Core Gui Widgets)
target_link_libraries(ImageReaderDicomCollection ${VTK_LIBRARIES})
