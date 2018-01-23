/*
**    CPE Lyon
**    2018 Camille FARINEAU / Nicolas Ranc
**    Projet Majeur - Virtual Endoscopy
**
**    DICOMIMageReaderFileCollection.cxx
**    Main application :
**          Read DICOM data
**          Create GUI : 4 viewers - 3 vtkResliceImageViewer + 1 vtkGenericOpenGLRenderWindow
**          Each vtkResliceImageViewer window show one slice corresponding to one slide of the volume according to an axis (X,Y,Z)
**          This volume is the contraction of all slides in accordance with all axis.
**          vtkGenericOpenGLRenderWindow window represents the surface of the volume created by Marching Cubes Algorithm.
**
*/


#ifndef __APPLE_CC__
#include <GL/glut.h>
#else
#include <glut.h>
#endif


#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
#include "vtkSMPTools.h"

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
//#include "vtkTestUtilities.h"
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkVoxelModeller.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include "vtkGPUVolumeRayCastMapper.h"
//#include "vtkTestErrorObserver.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkAxesActor.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include <vtksys/Process.h>

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
#include "vtkOpenGLRenderer.h"
#include "vtkOpenGLCamera.h"
#include "vtkCullerCollection.h"
#include "vtkLight.h"
#include "vtkImageShrink3D.h"
#include "vtkTimerLog.h"
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
#include "vtkSelectionNode.h"
#include "vtkExtractSelection.h"
#include "vtkSelection.h"
#include "vtkUnstructuredGrid.h"
#include "vtkGeometryFilter.h"
#include "vtkFileOutputWindow.h"

#include <QApplication>

#include <vtkActor.h>
#include <vtkGenericOpenGLRenderWindow.h>
//#include <vtkSmartVolumeMapper.h>


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

#include "vtkOutlineFilter.h"

vtkSmartPointer<vtkRenderer> surfaceRenderer = vtkSmartPointer<vtkRenderer>::New();
vtkSmartPointer<vtkGenericOpenGLRenderWindow> surfaceRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

vtkNew<vtkRenderer> ren;
vtkNew<vtkRenderWindow> renWin;

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

using namespace std;

vtkStandardNewMacro(KeyPressInteractorStyle);
vtkStandardNewMacro(KeyPressInteractorNavigationStyle);

/*static QMainWindow *QtVTKRenderWindows;*/


/*void Reshape( int width, int height )
{
  renWin->SetSize( width, height );
}

void Draw()
{
  QtVTKRenderWindows->show();
  surfaceRenderWindow->Render();
 // surfaceRenderer->GetActiveCamera()->Azimuth( 1 );
  glutPostRedisplay();
}*/


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


int main(int argc, char *argv[])
{
 if ( argc <= 1 )
  {
    cout << "Usage: " << argv[0] << " <dicom folder>" << endl;
    return 1;
  }

  vtkSmartPointer<vtkFileOutputWindow> fileOutputWindow =  vtkSmartPointer<vtkFileOutputWindow>::New();
    fileOutputWindow->SetFileName( "output_errors.txt" );

    // Note that the SetInstance function is a static member of vtkOutputWindow.
    vtkOutputWindow::SetInstance(fileOutputWindow);

  std::string directoryName = argv[1];
  vtkSmartPointer<vtkDICOMImageReader> DICOMReader = vtkSmartPointer<vtkDICOMImageReader>::New();
  // Read the input files
  DICOMReader->SetDirectoryName(directoryName.c_str());
  cout << "Directory name: " << DICOMReader->GetDirectoryName() << endl;
  DICOMReader->Update();
  std::cout<<"Extent: "<<DICOMReader->GetDataExtent()[0]<<" , "<<DICOMReader->GetDataExtent()[1]<<" , "<<DICOMReader->GetDataExtent()[2]<<" , "<<DICOMReader->GetDataExtent()[3]<<" , "<<DICOMReader->GetDataExtent()[4]<<" , "<<DICOMReader->GetDataExtent()[5]<<std::endl;


    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
    QApplication app(argc, argv);
    QMainWindow *QtVTKRenderWindows = new QMainWindow();
    //QtVTKRenderWindows->resize(1240, 850);
    QtVTKRenderWindows->resize(1050, 700);
    QWidget *main_widget = new QWidget();
    QWidget *gridLayoutWidget = new QWidget(main_widget);
    //gridLayoutWidget->setGeometry(QRect(10, 30, 1221, 791));
    gridLayoutWidget->setGeometry(QRect(10, 30, 1000, 650));
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


   /* // GLUT initialization
     glutInit( &argc, argv );
     glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
     glutCreateWindow( "VTK-GLUT Example") ;
     glutReshapeFunc( Reshape );
     glutDisplayFunc( Draw);
*/

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
      //style[i]->SetAnnotation(cornerAnnotation[i]);
      style[i]->SetPicker(propPicker[i]);

      if(i==0)
          view1->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );
      else if(i==1)
          view2->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );
      else if(i==2)
          view3->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );

      style[i]->AddObserver(vtkCommand::MouseMoveEvent, callback[i]);

      //style[i]->RemoveObserver(vtkCommand::KeyPressEvent);
     // style[i]->RemoveObserver(vtkCommand::CharEvent);

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
 // surface->SetValue(0,isoValue);


  double range[2];
  range[0]=-40.0f;
  range[1]=-50.0f;
  surface->GenerateValues(1,range);


    surface->Update();
    std::cout << "Before Decimation." << std::endl;
     std::cout << "There are: " << NumberofTriangles(surface->GetOutput())
               << " triangles." << std::endl;

     vtkSmartPointer<vtkDecimatePro> deci = vtkSmartPointer<vtkDecimatePro>::New();
     deci->SetInputConnection(surface->GetOutputPort());
     deci->SetTargetReduction(0.9);
     deci->SetAbsoluteError(0.01);
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

     /************************************/

     /***************************************/
  vtkSmartPointer<vtkStripper> surfaceStripper = vtkSmartPointer<vtkStripper>::New();
  surfaceStripper->SetInputData(surface->GetOutput());

//  vtkSmartPointer<vtkRenderer> surfaceRenderer = vtkSmartPointer<vtkRenderer>::New();
  surfaceRenderer->SetBackground(.1, .2, .3);

 // vtkSmartPointer<vtkGenericOpenGLRenderWindow> surfaceRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  surfaceRenderWindow->AddRenderer(surfaceRenderer);

  view4->SetRenderWindow(surfaceRenderWindow);
  vtkSmartPointer<vtkRenderWindowInteractor> surfaceInteractor = view4->GetInteractor();
  surfaceInteractor->SetRenderWindow(surfaceRenderWindow);



 double scalarRange[2];
/* vtkNew<vtkGPUVolumeRayCastMapper> volumeMapper;
 volumeMapper->SetInputConnection(surface->GetOutputPort());
 volumeMapper->SetSampleDistance(0.01);*/


 // Add outline filter
/* vtkNew<vtkOutlineFilter> outlineFilter;
 outlineFilter->SetInputConnection(reader->GetOutputPort());*/


  /*volumeMapper->GetInput()->GetScalarRange(scalarRange);
  volumeMapper->SetBlendModeToComposite();
  volumeMapper->SetAutoAdjustSampleDistances(1);*/

  /*// Force a number of partition blocks
  vtkOpenGLGPUVolumeRayCastMapper* mappergl =
  vtkOpenGLGPUVolumeRayCastMapper::SafeDownCast(mapper.GetPointer());
  mappergl->SetPartitions(2, 1, 2);*/

  /*vtkNew<vtkOutlineFilter> outlineFilter;
  outlineFilter->SetInputConnection(surface->GetOutputPort());*/


  //old version
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(surface->GetOutput());
  mapper->ScalarVisibilityOff();

 /* mapper->SetInputConnection(outlineFilter->GetOutputPort());*/

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
  actor->GetProperty()->SetSpecular(.45);
  actor->GetProperty()->SetSpecularPower(20);
  actor->GetProperty()->SetDiffuse(.9);
  //actor->GetProperty()->BackfaceCullingOn();

 /* volumeMapper->GetInput()->GetScalarRange(scalarRange);
  volumeMapper->SetBlendModeToComposite();
  volumeMapper->SetAutoAdjustSampleDistances(1);*/

  vtkSmartPointer<vtkCameraActor> cameraActor = vtkSmartPointer<vtkCameraActor>::New();
  cameraActor->SetCamera(aCamera);
  cameraActor->GetProperty()->SetColor(0, 0, 0);


  /*************************************************/

   vtkSmartPointer<vtkSphereSource> source = vtkSmartPointer<vtkSphereSource>::New();

   double radius = 0.7;
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
   actor_camera->GetProperty()->SetOpacity(0.0);
   surfaceRenderer->AddActor ( actor_camera );

  /*********************************************/

  surface->Update();
  //surfaceRenderer->AddViewProp(volume2);
  surfaceRenderer->AddActor(actor);
  surfaceRenderer->ResetCameraClippingRange();
  // Finally, add the volume to the renderer

  surfaceRenderWindow->Render();

  vtkSmartPointer<KeyPressInteractorNavigationStyle> styleNav =vtkSmartPointer<KeyPressInteractorNavigationStyle>::New();
  styleNav->SetCamera(aCamera);
  styleNav->SetInteractor(view4->GetInteractor());
 // styleNav->SetSurface(deci->GetOutput());
  styleNav->SetSurfaceCollision(surface);
  styleNav->SetSphere(source);
  styleNav->SetIntersectionPolyDataFilter();
  surfaceInteractor->SetInteractorStyle( styleNav );


  style[0]->SetStyleNav(styleNav);

  //P interaction windows 2 and 3
  /*style[1]->SetStyleNav(styleNav);
  style[2]->SetStyleNav(styleNav);*/


 for(int j=0;j<3;j++){
    style[j]->SetCamera(aCamera);
    style[j]->SetInteractor(surfaceInteractor);
    style[j]->SetViewerNav(surfaceInteractor->GetRenderWindow());
  }

  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  vtkSmartPointer<vtkOrientationMarkerWidget> widget =
  vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  widget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
  widget->SetOrientationMarker( axes );
  widget->SetInteractor( surfaceInteractor );
  widget->SetViewport( 0.0, 0.0, 0.2, 0.2 );
  widget->SetEnabled( 1 );
  widget->InteractiveOff();

  surfaceInteractor->Start();
  QtVTKRenderWindows->show();

 /* // Here is the trick: we ask the RenderWindow to join the current OpenGL context created by GLUT
  surfaceRenderWindow->InitializeFromCurrentContext();

  // Let's start the main GLUT rendering loop
  glutMainLoop();*/

  app.exec();
  return 0;
}
