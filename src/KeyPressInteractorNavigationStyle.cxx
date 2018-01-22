#include "KeyPressInteractorNavigationStyle.h"

KeyPressInteractorNavigationStyle::KeyPressInteractorNavigationStyle()
{
    this->Camera     = NULL;
    this->intersectionPolyDataFilter = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
    this->nb_inter = 0;
}


KeyPressInteractorNavigationStyle::~KeyPressInteractorNavigationStyle()
{
    this->Camera     = NULL;
}

void KeyPressInteractorNavigationStyle::SetCamera(const vtkSmartPointer<vtkCamera>& camera_){
    Camera=camera_;
}

void KeyPressInteractorNavigationStyle::SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& Interactor){
    this->Interactor=Interactor;
}


void KeyPressInteractorNavigationStyle::SetSurface(const vtkSmartPointer<vtkPolyData>& surface){
    this->Surface=surface;
}

void KeyPressInteractorNavigationStyle::SetSphere(const vtkSmartPointer<vtkSphereSource>& sphere){
    this->Sphere=sphere;
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
  Camera->SetDistance(1);

  // Handle an arrow key
  if(key == "Up")
    {
    Camera->Elevation(-1);
    }

  if(key == "Down")
    {
    Camera->Elevation(1);
    }

  if(key == "Left")
    {
    Camera->Azimuth(1);
    }

  if(key == "Right")
    {
    Camera->Azimuth(-1);
    }

  if(key == "z")
    {
    Camera->Dolly(5);
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    Sphere->SetCenter(Camera->GetPosition());
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    intersectionPolyDataFilter->Update();
    nb_inter = intersectionPolyDataFilter->GetNumberOfIntersectionPoints();
    std::cout<<"Inters: "<<nb_inter<<std::endl;
    if(nb_inter >= 1)
    {
       Camera->Dolly(0.2);
       std::cout<<"don't move"<<std::endl;
    }


    }
  if(key == "s")
    {
    Camera->Dolly(0.4);
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    Sphere->SetCenter(Camera->GetPosition());
    std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;
    intersectionPolyDataFilter->Update();
    nb_inter = intersectionPolyDataFilter->GetNumberOfIntersectionPoints();
    std::cout<<"Inters: "<<nb_inter<<std::endl;
    if(nb_inter >= 1)
    {
       Camera->Dolly(10);
       std::cout<<"don't move"<<std::endl;
    }



    }

  if(key == "Escape")
    {
    exit(0);
    }

  double dis[2]={0.5,1000.0};
  Camera->SetClippingRange(dis);
  Camera->SetDistance(1);
  Sphere->SetCenter(Camera->GetPosition());


  this->Interactor->GetRenderWindow()->Render();
 // vtkInteractorStyleTrackballCamera::OnKeyPress();
}
