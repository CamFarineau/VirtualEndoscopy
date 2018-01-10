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
#include <vtkRenderWindow.h>
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkInteractorStyleTrackballCamera.h"

//----------------------------------------------------------------------------


QtVTKRenderWindows::QtVTKRenderWindows( int vtkNotUsed(argc), char *argv[])
{
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


  //Marching Cubes
  double isoValue = -90;
  vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
  vtkSmartPointer<vtkImageData> volume = vtkSmartPointer<vtkImageData>::New();
  volume->DeepCopy(reader->GetOutput());
  surface->SetInputData(volume);
  surface->ComputeNormalsOn();
  surface->SetValue(0,isoValue);

  vtkSmartPointer<vtkRenderer> surfaceRenderer = vtkSmartPointer<vtkRenderer>::New();
  surfaceRenderer->SetBackground(.1, .2, .3);

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> surfaceRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  surfaceRenderWindow->AddRenderer(surfaceRenderer);

  this->ui->view4->SetRenderWindow(surfaceRenderWindow);
  vtkSmartPointer<vtkRenderWindowInteractor> surfaceInteractor = this->ui->view4->GetInteractor();//vtkSmartPointer<vtkRenderWindowInteractor>::New();
  surfaceInteractor->SetRenderWindow(surfaceRenderWindow);

  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(surface->GetOutputPort());
  mapper->ScalarVisibilityOff();

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  surfaceRenderer->AddActor(actor);

  //surfaceRenderWindow->Render();
  //surfaceInteractor->Start();

  //this->ui->view4->show();

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
};

void QtVTKRenderWindows::slotExit()
{
  qApp->exit();
}
