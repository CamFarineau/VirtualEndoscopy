#include "ui_QtVTKRenderWindows.h"
#include "QtVTKRenderWindows.h"

#include "vtkBoundedPlanePointPlacer.h"
#include "vtkCellPicker.h"
#include "vtkCommand.h"
#include <vtkCamera.h>
#include "vtkDICOMImageReader.h"
#include "vtkDistanceRepresentation.h"
#include "vtkDistanceRepresentation2D.h"
#include "vtkDistanceWidget.h"
#include <vtkGenericOpenGLRenderWindow.h>
#include "vtkHandleRepresentation.h"
#include "vtkImageData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageSlabReslice.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLookupTable.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkPointHandleRepresentation2D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkProperty.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkResliceImageViewerMeasurements.h"
#include "vtkMarchingCubes.h"
#include <unistd.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include <vtkVertexGlyphFilter.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkInteractorStyleJoystickCamera.h>
#include <vtkRenderWindow.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif


//static std::map <std::string,bool> keyboard;

/*void QtVTKRenderWindows::init_map(std::map <std::string,bool>& map){
    map["z"]=0;
    map["s"]=0;
    map["q"]=0;
    map["d"]=0;

    map["Up"]=0;
    map["Down"]=0;
    map["Left"]=0;
    map["Right"]=0;
}*/

//----------------------------------------------------------------------------

// Define interaction style
class customMouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static customMouseInteractorStyle* New();
    vtkSmartPointer<vtkCamera> camera;
    vtkTypeMacro(customMouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    void SetCamera(const vtkSmartPointer<vtkCamera>& camera_){
        camera=camera_;
    }

    virtual void OnLeftButtonDown()
    {
      std::cout << "Pressed left mouse button." << std::endl;
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    }

    virtual void OnMiddleButtonDown()
    {
      std::cout << "Pressed middle mouse button." << std::endl;
      // Forward events
      vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
    }

    virtual void OnRightButtonDown()
    {
      std::cout << "Pressed right mouse button." << std::endl;
      // Forward events
      /*double *position = camera->GetPosition();
      double rotation = camera->GetRoll();

      position[0] += 100 * cos(rotation * M_PI / 180.0);
      position[1] += 100 * sin(rotation * M_PI / 180.0);
      camera->SetPosition(position);*/

      //camera->SetPosition(500.0f,500.0f,500.0f);
      vtkInteractorStyleTrackballCamera::OnRightButtonDown();
    }

     virtual void OnMouseMove()
    {
        //std::cout<<"rotation : "<<camera->GetRoll()<<std::endl;
        vtkInteractorStyleTrackballCamera::OnMouseMove();
    }

};

vtkStandardNewMacro(customMouseInteractorStyle);


// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static KeyPressInteractorStyle* New();
    vtkSmartPointer<vtkCamera> camera;
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

    void SetCamera(const vtkSmartPointer<vtkCamera>& camera_){
        camera=camera_;
    }

    virtual void OnKeyPress()
    {
      // Get the keypress
      vtkRenderWindowInteractor *rwi = this->Interactor;
      std::string key = rwi->GetKeySym();

      // Output the key that was pressed
      std::cout << "Pressed " << key << std::endl;

      // Handle an arrow key
      if(key == "Up")
        {
        std::cout << "The up arrow was pressed." << std::endl;
        camera->Elevation(1);
        //this->Interactor->GetRenderWindow()->Render();
        }

      if(key == "Down")
        {
        std::cout << "The up arrow was pressed." << std::endl;
        camera->Elevation(-1);
        //this->Interactor->GetRenderWindow()->Render();

        }

      if(key == "Left")
        {
        std::cout << "The up arrow was pressed." << std::endl;
        camera->Yaw(1);
        //this->Interactor->GetRenderWindow()->Render();
        }

      if(key == "Right")
        {
        std::cout << "The up arrow was pressed." << std::endl;
        //camera->Elevation(-2);
        camera->Yaw(-1);
        //this->Interactor->GetRenderWindow()->Render();

        }

      if(key == "z")
        {
        std::cout << "Z pressed." << std::endl;
        camera->Dolly(5);

        }
      if(key == "s")
        {
        std::cout << "Z pressed." << std::endl;
        camera->Dolly(0.5);

        }
      if(key == "q")
        {
        std::cout << "Z pressed." << std::endl;

        }
      if(key == "d")
        {
        std::cout << "Z pressed." << std::endl;

        }

      if(key == "Escape")
        {
            exit(0);
        }


      camera->SetDistance(1);

      this->Interactor->GetRenderWindow()->Render();
      // Forward events
      vtkInteractorStyleTrackballCamera::OnKeyPress();
    }

};
vtkStandardNewMacro(KeyPressInteractorStyle);

QtVTKRenderWindows::QtVTKRenderWindows( int vtkNotUsed(argc), char *argv[])
{
 // init_map(keyboard);


  this->ui = new Ui_QtVTKRenderWindows;
  this->ui->setupUi(this);
  vtkSmartPointer< vtkDICOMImageReader > reader = vtkSmartPointer< vtkDICOMImageReader >::New();
  reader->SetDirectoryName(argv[1]);
  reader->Update();

  for (int i = 0; i < 3; i++)
  {
    riw[i] = vtkSmartPointer< vtkResliceImageViewer >::New();
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    riw[i]->SetRenderWindow(renderWindow);
    riw[i]->SetInputData(reader->GetOutput());
  }

  this->ui->view1->SetRenderWindow(riw[0]->GetRenderWindow());
  riw[0]->SetupInteractor(this->ui->view1->GetRenderWindow()->GetInteractor());

  this->ui->view2->SetRenderWindow(riw[1]->GetRenderWindow());
  riw[1]->SetupInteractor(this->ui->view2->GetRenderWindow()->GetInteractor());

  this->ui->view3->SetRenderWindow(riw[2]->GetRenderWindow());
  riw[2]->SetupInteractor(this->ui->view3->GetRenderWindow()->GetInteractor());

  for (int i = 0; i < 3; i++)
  {
    riw[i]->SetSliceOrientation(i);
    riw[i]->Render();
    riw[i]->GetRenderer()->ResetCamera();
  }

  vtkSmartPointer< vtkCamera > camera[2];

  for (int i = 0; i < 3; i++)
  {
    if(i<2){
       camera[i] =  riw[i]->GetRenderer()->GetActiveCamera();
       double rotation = 180.0;
       camera[i]->Roll(rotation);
    }
  }

  this->ui->view1->show();
  this->ui->view2->show();
  this->ui->view3->show();


  /** Volume */
  //Marching Cubes
  double isoValue = 200;
  vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
  vtkSmartPointer<vtkImageData> volume = vtkSmartPointer<vtkImageData>::New();
  volume->DeepCopy(reader->GetOutput());
  surface->SetInputData(volume);
  surface->ComputeNormalsOn();
  surface->SetValue(0,isoValue);

  //Mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper1->SetInputConnection(surface->GetOutputPort());
  mapper1->ScalarVisibilityOff();

  //Actors
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper1);




  /** Points*/

  vtkSmartPointer<vtkPoints> points =vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint (1.0, 0.0, 0.0);
  points->InsertNextPoint (2.0, 0.0, 0.0);
  points->InsertNextPoint (3.0, 0.0, 0.0);
  points->InsertNextPoint (4.0, 0.0, 0.0);
  points->InsertNextPoint (5.0, 0.0, 0.0);
  points->InsertNextPoint (6.0, 0.0, 0.0);
  points->InsertNextPoint (7.0, 0.0, 0.0);
  points->InsertNextPoint (8.0, 0.0, 0.0);
  points->InsertNextPoint (9.0, 0.0, 0.0);
  points->InsertNextPoint (10.0, 0.0, 0.0);

  points->InsertNextPoint (0.0, 1.0, 0.0);
  points->InsertNextPoint (0.0, 2.0, 0.0);
  points->InsertNextPoint (0.0, 3.0, 0.0);
  points->InsertNextPoint (0.0, 4.0, 0.0);
  points->InsertNextPoint (0.0, 5.0, 0.0);
  points->InsertNextPoint (0.0, 6.0, 0.0);
  points->InsertNextPoint (0.0, 7.0, 0.0);
  points->InsertNextPoint (0.0, 8.0, 0.0);
  points->InsertNextPoint (0.0, 9.0, 0.0);
  points->InsertNextPoint (0.0, 10.0, 0.0);

  points->InsertNextPoint (0.0, 0.0, 1.0);
  points->InsertNextPoint (0.0, 0.0, 2.0);
  points->InsertNextPoint (0.0, 0.0, 3.0);
  points->InsertNextPoint (0.0, 0.0, 4.0);
  points->InsertNextPoint (0.0, 0.0, 5.0);
  points->InsertNextPoint (0.0, 0.0, 6.0);
  points->InsertNextPoint (0.0, 0.0, 7.0);
  points->InsertNextPoint (0.0, 0.0, 8.0);
  points->InsertNextPoint (0.0, 0.0, 9.0);
  points->InsertNextPoint (0.0, 0.0, 10.0);

  vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
  pointsPolydata->SetPoints(points);

  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();

  vertexFilter->SetInputData(pointsPolydata);
  vertexFilter->Update();

  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->ShallowCopy(vertexFilter->GetOutput());

     // Setup colors
     unsigned char red[3] = {255, 0, 0};
     unsigned char green[3] = {0, 255, 0};
     unsigned char blue[3] = {0, 0, 255};

     vtkSmartPointer<vtkUnsignedCharArray> colors =vtkSmartPointer<vtkUnsignedCharArray>::New();
     colors->SetNumberOfComponents(3);
     colors->SetName ("Colors");
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);
     colors->InsertNextTupleValue(red);

     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);
     colors->InsertNextTupleValue(green);

     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);
     colors->InsertNextTupleValue(blue);

     polydata->GetPointData()->SetScalars(colors);

  //Mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper2= vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper2->SetInputData(polydata);

  //Actor
  vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
  actor2->SetMapper(mapper2);


  /** Camera*/
  vtkSmartPointer<vtkCamera> aCamera =   vtkSmartPointer<vtkCamera>::New();
    aCamera->SetPosition(450,
                         1000,
                         0);
    aCamera->SetFocalPoint(250,
                           250,
                           70);
   aCamera->SetViewUp(0,0,-1);

   aCamera->SetFocalDisk(0);


  /** Renderer*/
  vtkSmartPointer<vtkRenderer> surfaceRenderer = vtkSmartPointer<vtkRenderer>::New();
  surfaceRenderer->SetBackground(.1, .2, .3);
  //surfaceRenderer->SetBackground(1, 1, 1);
  surfaceRenderer->SetActiveCamera(aCamera);



  /** Window Renderer*/
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> surfaceRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
 //  vtkSmartPointer<vtkRenderWindow> surfaceRenderWindow = vtkSmartPointer<vtkRenderWindow>::New();

  surfaceRenderWindow->AddRenderer(surfaceRenderer);

  this->ui->view4->SetRenderWindow(surfaceRenderWindow);
  this->ui->view4->show();

  /** Interactor*/
  vtkSmartPointer<vtkRenderWindowInteractor> surfaceInteractor = this->ui->view4->GetInteractor();//vtkSmartPointer<vtkRenderWindowInteractor>::New();
  surfaceInteractor->SetRenderWindow(surfaceRenderWindow);

  surfaceRenderer->AddActor(actor);
  surfaceRenderer->AddActor(actor2);


  surfaceRenderWindow->Render();

  vtkSmartPointer<KeyPressInteractorStyle> style =vtkSmartPointer<KeyPressInteractorStyle>::New();
  style->SetCamera(aCamera);
  surfaceInteractor->SetInteractorStyle( style );


  surfaceInteractor->Start();

  this->ui->view4->show();


  /*******************************************************/
  /*OLD CODE*/

  // Set up action signals and slots
  /*connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
  connect(this->ui->resliceModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(resliceMode(int)));
  connect(this->ui->thickModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(thickMode(int)));
  this->ui->thickModeCheckBox->setEnabled(0);

  connect(this->ui->radioButton_Max, SIGNAL(pressed()), this, SLOT(SetBlendModeToMaxIP()));
  connect(this->ui->radioButton_Min, SIGNAL(pressed()), this, SLOT(SetBlendModeToMinIP()));
  connect(this->ui->radioButton_Mean, SIGNAL(pressed()), this, SLOT(SetBlendModeToMeanIP()));
  this->ui->blendModeGroupBox->setEnabled(0);

  connect(this->ui->resetButton, SIGNAL(pressed()), this, SLOT(ResetViews()));
  connect(this->ui->AddDistance1Button, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementToView1()));*/
}

void QtVTKRenderWindows::slotExit()
{
  qApp->exit();
}
