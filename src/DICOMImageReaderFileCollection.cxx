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

#include "KeyPressInteractorStyle.h"
#include "KeyPressInteractorNavigationStyle.h"
#include "vtkImageInteractionCallback.h"

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
#include "vtkAxesActor.h"


#include <vtkVersion.h>
#include <vtkAssemblyPath.h>
#include <vtkCell.h>
#include <vtkCommand.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageActor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPointData.h>
#include <vtkPropPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkImageNoiseSource.h>
#include <vtkImageCast.h>
#include <vtkMath.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkUnsignedCharArray.h>
#include <vtkOrientationMarkerWidget.h>


#include "vtkResliceImageViewer.h"
#include "vtkOBBTree.h"
#include <vtkProperty.h>
#include <vtkStripper.h>
#include "vtkOutlineFilter.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkCameraActor.h"
#include "vtkCallbackCommand.h"
#include "vtkSelectEnclosedPoints.h"
#include "vtkDecimatePro.h"
#include "vtkSliderRepresentation2D.h"
#include "vtkSliderWidget.h"
#include <vtkObjectFactory.h>
#include "vtkSphereSource.h"

#include <QApplication>

#include <vtkActor.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <QVTKOpenGLWidget.h>
#include <QSurfaceFormat>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include "QVTKOpenGLWidget.h"


// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

using namespace std;

vtkStandardNewMacro(KeyPressInteractorStyle);
vtkStandardNewMacro(KeyPressInteractorNavigationStyle);


auto NumberofTriangles = [](vtkPolyData* pd) {
  vtkCellArray* cells = pd->GetPolys();
  vtkIdType npts = 0;
  vtkIdType* pts;
  auto numOfTriangles = 0;
  for (auto i = 0; i < pd->GetNumberOfPolys(); ++i)
  {
    int c = cells->GetNextCell(npts, pts);
    if (c == 0)
    {
      break;
    }
    // If a cell has three points it is a triangle.
    if (npts == 3)
    {
      numOfTriangles++;
    }
  }
  return numOfTriangles;
};


class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback *New()
    {
    return new vtkSliderCallback;
    }

  vtkSliderCallback():OBBTree(0), Level(0), PolyData(0), Renderer(0){}

  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
    vtkSliderWidget *sliderWidget =
      reinterpret_cast<vtkSliderWidget*>(caller);
    this->Level = vtkMath::Round(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
    this->OBBTree->GenerateRepresentation(this->Level, this->PolyData);
    this->Renderer->Render();
    }

  vtkSmartPointer<vtkOBBTree> OBBTree;
  int Level;
  vtkSmartPointer<vtkPolyData> PolyData;
  vtkSmartPointer<vtkRenderer> Renderer;
};

int main(int argc, char *argv[])
{

  if ( argc <= 1 )
  {
    cout << "Usage: " << argv[0] << " <dicom folder>" << endl;
    return 1;
  }

  std::string directoryName = argv[1];
  vtkSmartPointer<vtkDICOMImageReader> DICOMReader = vtkSmartPointer<vtkDICOMImageReader>::New();
  // Read the input files
  DICOMReader->SetDirectoryName(directoryName.c_str());
  cout << "Directory name: " << DICOMReader->GetDirectoryName() << endl;
  DICOMReader->Update();


    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
    QApplication app(argc, argv);
    QMainWindow *QtVTKRenderWindows = new QMainWindow();
    QtVTKRenderWindows->resize(1240, 850);
    QWidget *main_widget = new QWidget();
    QWidget *gridLayoutWidget = new QWidget(main_widget);
    gridLayoutWidget->setGeometry(QRect(10, 30, 1221, 791));
    QGridLayout *gridLayout_2 = new QGridLayout(gridLayoutWidget);
    gridLayout_2->setContentsMargins(0, 0, 0, 0);
    QVTKOpenGLWidget *view2 = new QVTKOpenGLWidget(gridLayoutWidget);
    gridLayout_2->addWidget(view2, 1, 0, 1, 1);
    QVTKOpenGLWidget *view4 = new QVTKOpenGLWidget(gridLayoutWidget);
    view4->setObjectName(QStringLiteral("view4"));
    gridLayout_2->addWidget(view4, 0, 1, 1, 1);
    QVTKOpenGLWidget *view3 = new QVTKOpenGLWidget(gridLayoutWidget);
    gridLayout_2->addWidget(view3, 1, 1, 1, 1);
    QVTKOpenGLWidget* view1 = new QVTKOpenGLWidget(gridLayoutWidget);
    gridLayout_2->addWidget(view1, 0, 0, 1, 1);
    QtVTKRenderWindows->setCentralWidget(main_widget);

    vtkSmartPointer<vtkResliceImageViewer> imageViewer[3];

    // Visualize
    for(int i=0; i<3; i++)
    {
         imageViewer[i] = vtkSmartPointer<vtkResliceImageViewer>::New();
         imageViewer[i]->SetInputData(DICOMReader->GetOutput());
         vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
         imageViewer[i]->SetRenderWindow(renderWindow);
    }



  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  //imageViewer->SetupInteractor(widget.GetRenderWindow()->GetInteractor());

  view1->SetRenderWindow(imageViewer[0]->GetRenderWindow());
  imageViewer[0]->SetupInteractor(view1->GetInteractor());
  view2->SetRenderWindow(imageViewer[1]->GetRenderWindow());
  imageViewer[1]->SetupInteractor(view2->GetInteractor());
  view3->SetRenderWindow(imageViewer[2]->GetRenderWindow());
  imageViewer[2]->SetupInteractor(view3->GetInteractor());

    //imageViewer->GetRenderWindow()->SetSize(800,800);
  vtkSmartPointer<vtkCamera> camera[2];
  vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation[3];
  vtkSmartPointer<vtkImageInteractionCallback> callback[3];
  vtkSmartPointer<vtkPropPicker> propPicker[3];
  vtkImageActor* imageActor[3];
  vtkSmartPointer<KeyPressInteractorStyle> style[3];
  for (int i = 0; i < 3; i++)
  {
      // Picker
      // Get pixel coordinates
      // Picker to pick pixels
      propPicker[i] = vtkSmartPointer<vtkPropPicker>::New();
      propPicker[i]->PickFromListOn();

      // Give the picker a prop to pick
      imageActor[i] = imageViewer[i]->GetImageActor();
      propPicker[i]->AddPickList(imageActor[i]);

      // disable interpolation, so we can see each pixel
      imageActor[i]->InterpolateOff();


      // Annotate the image with window/level and mouse over pixel
      // information
      cornerAnnotation[i] = vtkSmartPointer<vtkCornerAnnotation>::New();
      cornerAnnotation[i]->SetLinearFontScaleFactor(2);
      cornerAnnotation[i]->SetNonlinearFontScaleFactor(1);
      cornerAnnotation[i]->SetMaximumFontSize(20);
      cornerAnnotation[i]->SetText(0, "Off Image");
      //cornerAnnotation[i]->SetText(3, "<window>\n<level>");
      cornerAnnotation[i]->GetTextProperty()->SetColor(1, 0, 0);

      imageViewer[i]->GetRenderer()->AddViewProp(cornerAnnotation[i]);

      // Callback listens to MouseMoveEvents invoked by the interactor's style
      callback[i] = vtkSmartPointer<vtkImageInteractionCallback>::New();
      callback[i]->SetViewer(imageViewer[i]);
      callback[i]->SetAnnotation(cornerAnnotation[i]);
      callback[i]->SetPicker(propPicker[i]);

      style[i] = vtkSmartPointer<KeyPressInteractorStyle>::New();
      style[i]->SetViewer(imageViewer[i]);
      style[i]->SetAnnotation(cornerAnnotation[i]);
      style[i]->SetPicker(propPicker[i]);

      if(i==0)
          view1->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );
      else if(i==1)
          view2->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );
      else if(i==2)
          view3->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );

      style[i]->AddObserver(vtkCommand::MouseMoveEvent, callback[i]);


    imageViewer[i]->SetSliceOrientation(i);
    imageViewer[i]->SetColorLevel(-100);
    imageViewer[i]->SetColorWindow(4000);
    imageViewer[i]->Render();
    imageViewer[i]->GetRenderer()->ResetCamera();

    if(i<2){
       camera[i] =  imageViewer[i]->GetRenderer()->GetActiveCamera();
       double rotation = 180.0;
       camera[i]->Roll(rotation);
    }
  }

  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();


  //Marching Cubes
  double isoValue=-90;
  /*std::cout<<"Please enter an isoValue for the Marching Cubes: ";
  std::cin>>isoValue;
  std::cout<<"The value you entered is "<<isoValue<<std::endl;*/


  vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
  vtkSmartPointer<vtkImageData> volume = vtkSmartPointer<vtkImageData>::New();
  volume->DeepCopy(DICOMReader->GetOutput());


  double middlePoint[3];


  surface->SetInputData(volume);
  surface->ComputeNormalsOn();
  surface->ComputeGradientsOn();
  surface->ComputeScalarsOn();
  surface->SetValue(0,isoValue);
  //surface->GenerateValues(5,-120,0);

    surface->Update();
    std::cout << "Before Decimation." << std::endl;
     std::cout << "There are: " << NumberofTriangles(surface->GetOutput())
               << " triangles." << std::endl;

     vtkSmartPointer<vtkDecimatePro> deci = vtkSmartPointer<vtkDecimatePro>::New();
     deci->SetInputConnection(surface->GetOutputPort());
     deci->SetTargetReduction(0.95);
     deci->SetAbsoluteError(0.05);
     deci->SetFeatureAngle(30);
     deci->SetErrorIsAbsolute(1);
     deci->AccumulateErrorOn();
     deci->PreserveTopologyOff();
     deci->SplittingOn();
     deci->BoundaryVertexDeletionOn();
     // deci->SplittingOff();
     deci->Update();
     std::cout << "After Decimation." << std::endl;
     std::cout << "There are: " << NumberofTriangles(deci->GetOutput())
               << " triangles." << std::endl;


  vtkSmartPointer<vtkStripper> surfaceStripper = vtkSmartPointer<vtkStripper>::New();
  surfaceStripper->SetInputConnection(deci->GetOutputPort());

  vtkSmartPointer<vtkRenderer> surfaceRenderer = vtkSmartPointer<vtkRenderer>::New();
  surfaceRenderer->SetBackground(.1, .2, .3);

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> surfaceRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  surfaceRenderWindow->AddRenderer(surfaceRenderer);

  view4->SetRenderWindow(surfaceRenderWindow);
  vtkSmartPointer<vtkRenderWindowInteractor> surfaceInteractor = view4->GetInteractor();
  surfaceInteractor->SetRenderWindow(surfaceRenderWindow);
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(surfaceStripper->GetOutputPort());
  mapper->ScalarVisibilityOff();

  double bounds[6];
  mapper->GetBounds(bounds);
  //surface->GetOutput()->GetBounds(bounds);
  middlePoint[0] = (bounds[1] + bounds[0])/2;
  middlePoint[1] = (bounds[3] + bounds[2])/2;
  middlePoint[2] = (bounds[5] + bounds[4])/2;

  vtkSmartPointer<vtkCamera> aCamera =   vtkSmartPointer<vtkCamera>::New();
  aCamera =  surfaceRenderer->GetActiveCamera();

  aCamera->SetPosition(450, 1000, 0);
  aCamera->SetFocalPoint(middlePoint[0], middlePoint[1],middlePoint[2]);
  aCamera->SetViewUp(0,0,-1);


  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(1, .49, .25);
  actor->GetProperty()->SetSpecular(.3);
  actor->GetProperty()->SetSpecularPower(20);

  //actor->GetProperty()->BackfaceCullingOn();

  vtkSmartPointer<vtkCameraActor> cameraActor = vtkSmartPointer<vtkCameraActor>::New();
  cameraActor->SetCamera(aCamera);
  cameraActor->GetProperty()->SetColor(0, 0, 0);




  /*************************************************/

   vtkSmartPointer<vtkSphereSource> source = vtkSmartPointer<vtkSphereSource>::New();

   double radius = 1.0;
   source->SetRadius(radius);
   source->SetCenter(aCamera->GetPosition());
   source->SetPhiResolution(11);
   source->SetThetaResolution(21);
   vtkSmartPointer<vtkPolyDataMapper> mapper_camera =
           vtkSmartPointer<vtkPolyDataMapper>::New();
   mapper_camera->SetInputConnection ( source->GetOutputPort());
   vtkSmartPointer<vtkActor> actor_camera =
           vtkSmartPointer<vtkActor>::New();
   actor_camera->SetMapper ( mapper_camera );
   double r, g, b;
   r = vtkMath::Random(.4, 1.0);
   g = vtkMath::Random(.4, 1.0);
   b = vtkMath::Random(.4, 1.0);
   actor_camera->GetProperty()->SetDiffuseColor(r, g, b);
   actor_camera->GetProperty()->SetDiffuse(.2);
   actor_camera->GetProperty()->SetSpecular(.2);
   actor_camera->GetProperty()->SetSpecularColor(1.0,1.0,1.0);
   actor_camera->GetProperty()->SetSpecularPower(30.0);
   actor_camera->GetProperty()->SetOpacity(0.4);
   surfaceRenderer->AddActor ( actor_camera );

  /*********************************************/

  deci->Update();
  surfaceRenderer->AddActor(actor);
  surfaceRenderer->ResetCameraClippingRange();
  // Finally, add the volume to the renderer

  surfaceRenderWindow->Render();

  vtkSmartPointer<KeyPressInteractorNavigationStyle> styleNav =vtkSmartPointer<KeyPressInteractorNavigationStyle>::New();
  styleNav->SetCamera(aCamera);
  styleNav->SetInteractor(view4->GetInteractor());
  styleNav->SetSurface(surface->GetOutput());
  styleNav->SetSphere(source);
  styleNav->SetInteractionPolyDataFilter();
  surfaceInteractor->SetInteractorStyle( styleNav );

 for(int j=0;j<3;j++){
    style[j]->SetCamera(aCamera);
    style[j]->SetInteractor(surfaceInteractor);
  }

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  vtkSmartPointer<vtkOrientationMarkerWidget> widget =
  vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  widget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
  widget->SetOrientationMarker( axes );
  widget->SetInteractor( surfaceInteractor );
  widget->SetViewport( 0.0, 0.0, 0.2, 0.2 );
  widget->SetEnabled( 1 );
  widget->InteractiveOn();


  std::cout<<"Cam deb: X: "<<aCamera->GetPosition()[0]<<" Y: "<<aCamera->GetPosition()[1]<<" Z: "<<aCamera->GetPosition()[2]<<std::endl;

  surfaceInteractor->Start();
  QtVTKRenderWindows->show();

  app.exec();
  return 0;
}
