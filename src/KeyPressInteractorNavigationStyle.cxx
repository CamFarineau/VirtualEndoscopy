#include "KeyPressInteractorNavigationStyle.h"

KeyPressInteractorNavigationStyle::KeyPressInteractorNavigationStyle()
{
    this->camera     = NULL;
    this->intersectionPolyDataFilter = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
}


KeyPressInteractorNavigationStyle::~KeyPressInteractorNavigationStyle()
{
    this->camera     = NULL;
}

void KeyPressInteractorNavigationStyle::SetCamera(const vtkSmartPointer<vtkCamera>& camera_){
    camera=camera_;
}

void KeyPressInteractorNavigationStyle::SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& Interactor){
    this->Interactor=Interactor;

}


void KeyPressInteractorNavigationStyle::SetSurface(const vtkSmartPointer<vtkPolyData>& surface){
    Surface=surface;
}

void KeyPressInteractorNavigationStyle::SetSphere(const vtkSmartPointer<vtkSphereSource>& sphere){
    Sphere=sphere;
}

void KeyPressInteractorNavigationStyle::SetInteractionPolyDataFilter()
{
    intersectionPolyDataFilter->SetInputData(0, Sphere->GetOutput());
    intersectionPolyDataFilter->SetInputData(1, Surface);
    intersectionPolyDataFilter->Update();
}

void KeyPressInteractorNavigationStyle::OnKeyPress()
{
  // Get the keypress
  vtkRenderWindowInteractor *rwi = this->Interactor;
  std::string key = rwi->GetKeySym();
  camera->SetDistance(1.0);

  // Output the key that was pressed
  std::cout << "Pressed " << key << std::endl;
  camera->SetDistance(1);

  // Handle an arrow key
  if(key == "Up")
    {
    camera->Elevation(-1);
    }

  if(key == "Down")
    {
    camera->Elevation(1);
    }

  if(key == "Left")
    {
    camera->Yaw(1);
    }

  if(key == "Right")
    {
    camera->Yaw(-1);
    }

  if(key == "z")
    {
    camera->Dolly(5);
    }
  if(key == "s")
    {
    camera->Dolly(0.5);
    }

  if(key == "Escape")
    {
    exit(0);
    }
  double dis[2]={0.5,1000.0};
  camera->SetClippingRange(dis);
  camera->SetDistance(1);

  Sphere->SetCenter(camera->GetPosition());
  //intersectionPolyDataFilter->Update();
  std::cout<<"Inters: "<<intersectionPolyDataFilter->GetNumberOfIntersectionPoints()<<std::endl;

  this->Interactor->GetRenderWindow()->Render();
  //std::cout<<"X: "<<camera->GetPosition()[0]<<" , Y: "<<camera->GetPosition()[1]<<" , Z: "<<camera->GetPosition()[2]<<std::endl;



//  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//  points->InsertNextPoint(camera->GetPosition());

//  vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
//  pointsPolydata->SetPoints(points);

//  //Points inside test
//  vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints = vtkSmartPointer<vtkSelectEnclosedPoints>::New();

//  selectEnclosedPoints->SetInputData(pointsPolydata);
//  selectEnclosedPoints->SetSurfaceData(Surface);
//  selectEnclosedPoints->Update();

//  if(selectEnclosedPoints->IsInside(0))
//  {
//      std::cout << "Inside"  << std::endl;
//  }




  vtkInteractorStyleTrackballCamera::OnKeyPress();
}
