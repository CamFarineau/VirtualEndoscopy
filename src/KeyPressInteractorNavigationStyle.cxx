#include "KeyPressInteractorNavigationStyle.h"

KeyPressInteractorNavigationStyle::KeyPressInteractorNavigationStyle()
{
    this->camera     = NULL;
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


void KeyPressInteractorNavigationStyle::OnKeyPress()
{
  // Get the keypress
  vtkRenderWindowInteractor *rwi = this->Interactor;
  std::string key = rwi->GetKeySym();

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
  this->Interactor->GetRenderWindow()->Render();
  vtkInteractorStyleTrackballCamera::OnKeyPress();
}
