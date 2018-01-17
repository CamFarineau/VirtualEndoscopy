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

void KeyPressInteractorNavigationStyle::SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& interactor){
    Interactor=interactor;
}


void KeyPressInteractorNavigationStyle::OnKeyPress()
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
