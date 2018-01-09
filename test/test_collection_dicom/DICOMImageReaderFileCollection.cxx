/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestDICOMImageReaderFileCollection.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Test of vtkDICOMImageReader
// .SECTION Description
//


#include "vtkSmartPointer.h"

#include "vtkDICOMImageReader.h"

#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTestUtilities.h"
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkVoxelModeller.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkTestErrorObserver.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkInteractorStyleTrackballCamera.h"


int main(int argc, char *argv[])
{

  /*if ( argc <= 1 )
  {
    cout << "Usage: " << argv[0] << " <dicom folder>" << endl;
    return 1;
  }*/

  //char* dirName = vtkTestUtilities::ExpandDataFileName( argc, argv, "Data/dicom/collection" );

  std::string directoryName = argv[1];
  //delete [] dirName;

  vtkSmartPointer<vtkDICOMImageReader> DICOMReader =
    vtkSmartPointer<vtkDICOMImageReader>::New();

  // Read the input files
  DICOMReader->SetDirectoryName(directoryName.c_str());
  cout << "Directory name: " << DICOMReader->GetDirectoryName() << endl;

  DICOMReader->Update();

  // Read and display the image properties
  const char* fileExtensions = DICOMReader->GetFileExtensions();
  cout << "File extensions: " << fileExtensions << endl;

  const char* descriptiveName = DICOMReader->GetDescriptiveName();
  cout << "Descriptive name: " << descriptiveName << endl;

  double* pixelSpacing = DICOMReader->GetPixelSpacing();
  cout << "Pixel spacing: " << *pixelSpacing << endl;

  int width = DICOMReader->GetWidth();
  cout << "Image width: " << width << endl;

  int height = DICOMReader->GetHeight();
  cout << "Image height: " << height << endl;

  float* imagePositionPatient = DICOMReader->GetImagePositionPatient();
  cout << "Image position patient: " << *imagePositionPatient << endl;

  float* imageOrientationPatient = DICOMReader->GetImageOrientationPatient();
  cout << "Image orientation patient: " << *imageOrientationPatient << endl;

  int bitsAllocated = DICOMReader->GetBitsAllocated();
  cout << "Bits allocated: " << bitsAllocated << endl;

  int pixelRepresentation = DICOMReader->GetPixelRepresentation();
  cout << "Pixel representation: " << pixelRepresentation << endl;

  int numberOfComponents = DICOMReader->GetNumberOfComponents();
  cout << "Number of components: " << numberOfComponents << endl;

  const char* transferSyntaxUID = DICOMReader->GetTransferSyntaxUID();
  cout << "Transfer syntax UID: " << transferSyntaxUID << endl;

  float rescaleSlope = DICOMReader->GetRescaleSlope();
  cout << "Rescale slope: " << rescaleSlope << endl;

  float rescaleOffset = DICOMReader->GetRescaleOffset();
  cout << "Rescale offset: " << rescaleOffset << endl;

  const char* patientName = DICOMReader->GetPatientName();
  cout << "Patient name: " << patientName << endl;

  const char* studyUID = DICOMReader->GetStudyUID();
  cout << "Study UID: " << studyUID << endl;

  const char* studyID = DICOMReader->GetStudyID();
  cout << "Study ID: " << studyID << endl;

  float gantryAngle = DICOMReader->GetGantryAngle();
  cout << "Gantry angle: " << gantryAngle << endl;


  // Display the center slice
  int sliceNumber =
    (DICOMReader->GetOutput()->GetExtent()[5] +
     DICOMReader->GetOutput()->GetExtent()[4]) / 2;

  // Visualize
  /*
  vtkSmartPointer<vtkImageViewer2> imageViewer =
    vtkSmartPointer<vtkImageViewer2>::New();
  imageViewer->SetInputConnection(DICOMReader->GetOutputPort());
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  imageViewer->SetupInteractor(renderWindowInteractor);
  //imageViewer->SetSlice(sliceNumber);
  imageViewer->SetSliceOrientationToYZ();
  std::cout<<"imageViewer->GetSliceOrientation()"<<imageViewer->GetSliceOrientation()<<std::endl;
  std::cout<<"imageViewer->GetSliceMax()"<<imageViewer->GetSliceMax()<<std::endl;
  std::cout<<"imageViewer->GetSliceMin()"<<imageViewer->GetSliceMin()<<std::endl;

  imageViewer->SetSlice(200);
  imageViewer->Render();
  imageViewer->GetRenderer()->ResetCamera();
  vtkCamera *camera = imageViewer->GetRenderer()->GetActiveCamera();
  double angle = 180.0;
  camera->Roll(angle);
  renderWindowInteractor->Initialize();
  imageViewer->Render();

  renderWindowInteractor->Start();*/


  //Marching Cubes
  double isoValue = 800;
  vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
  vtkSmartPointer<vtkImageData> volume = vtkSmartPointer<vtkImageData>::New();
  volume->DeepCopy(DICOMReader->GetOutput());
  surface->SetInputData(volume);
  surface->ComputeNormalsOn();
  surface->SetValue(0,isoValue);

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(.1, .2, .3);

  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(renderWindow);

  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(surface->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  renderer->AddActor(actor);

  renderWindow->Render();
  interactor->Start();



  return 0;
}
