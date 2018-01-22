#include "KeyPressInteractorNavigationStyle.h"

KeyPressInteractorNavigationStyle::KeyPressInteractorNavigationStyle()
{
    this->camera     = NULL;
    this->intersectionPolyDataFilter = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
    this->nb_inter = 0;
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
    std::cout<<"Update surface for intersection"<<std::endl;
}

void KeyPressInteractorNavigationStyle::OnKeyPress()
{
  // Get the keypress
  vtkRenderWindowInteractor *rwi = this->Interactor;
  std::string key = rwi->GetKeySym();
  // Output the key that was pressed
  //std::cout << "Pressed " << key << std::endl;
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
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    Sphere->SetCenter(camera->GetPosition());
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    intersectionPolyDataFilter->Update();
    nb_inter = intersectionPolyDataFilter->GetNumberOfIntersectionPoints();
    std::cout<<"Inters: "<<nb_inter<<std::endl;
    if(nb_inter >= 1)
    {
       camera->Dolly(0.2);
       std::cout<<"don't move"<<std::endl;
    }


    }
  if(key == "s")
    {
    camera->Dolly(0.4);
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    Sphere->SetCenter(camera->GetPosition());
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    intersectionPolyDataFilter->Update();
    nb_inter = intersectionPolyDataFilter->GetNumberOfIntersectionPoints();
    std::cout<<"Inters: "<<nb_inter<<std::endl;
    if(nb_inter >= 1)
    {
       camera->Dolly(10);
       std::cout<<"don't move"<<std::endl;
    }



    }

  if(key == "Escape")
    {
    exit(0);
    }

  double dis[2]={0.5,1000.0};
  camera->SetClippingRange(dis);
  camera->SetDistance(1);
  Sphere->SetCenter(camera->GetPosition());


  this->Interactor->GetRenderWindow()->Render();




  vtkInteractorStyleTrackballCamera::OnKeyPress();
}
