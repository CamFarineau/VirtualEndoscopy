/*
**    CPE Lyon
**    Copyright (C) 2018 Camille FARINEAU / Nicolas Ranc
**    Projet Majeur - Virtual Endoscopy
**
**    KeyPressInteractorNavigationStyle.cxx
**    Interactor Style for the 3D Viewer : handle key event (to move the camera in the scene)
*/

#include "KeyPressInteractorNavigationStyle.h"

/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::KeyPressInteractorNavigationStyle()
 * Constructor
\*------------------------------------------------------------------------*/
KeyPressInteractorNavigationStyle::KeyPressInteractorNavigationStyle()
{
    this->Camera     = NULL;
    this->intersectionPolyDataFilter = vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
    this->nb_inter = 0;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::KeyPressInteractorNavigationStyle()
 * Destructor
\*------------------------------------------------------------------------*/
KeyPressInteractorNavigationStyle::~KeyPressInteractorNavigationStyle()
{
    this->Camera     = NULL;
}


/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::SetCamera
 * Set the camera use in the scene
 * Param: camera_: a vtkCamera
\*------------------------------------------------------------------------*/
void KeyPressInteractorNavigationStyle::SetCamera(const vtkSmartPointer<vtkCamera>& camera_){
    Camera=camera_;
}


/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::SetInteractor
 * Set the interactor of the RenderWindow. Useful to interact with the scene
 * Param: an interactor
\*------------------------------------------------------------------------*/
void KeyPressInteractorNavigationStyle::SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& interactor){
    this->Interactor=interactor;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::SetSurface()
 * Set the iso-surface created with Marching cubes (useful for collision between the camera bbox and the surface)
 * Param: surface: a vtkPolyData
\*------------------------------------------------------------------------*/
void KeyPressInteractorNavigationStyle::SetSurface(const vtkSmartPointer<vtkPolyData>& surface){
    this->Surface=surface;
}


/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::SetSphere()
 * Set the sphere that is used as a bbox for the camera (for the collision)
 * Param: sphere: a vtkSphereSource
\*------------------------------------------------------------------------*/
void KeyPressInteractorNavigationStyle::SetSphere(const vtkSmartPointer<vtkSphereSource>& sphere){
    this->Sphere=sphere;
}


/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::SetInteractionPolyDataFilter()
 * Set the 2 PolyData that will be used by the InteractionFilter (the bbox of the camera and the surface of the object of interest)
\*------------------------------------------------------------------------*/
void KeyPressInteractorNavigationStyle::SetIntersectionPolyDataFilter()
{
    // Set the first PolyData as the bbox of the camera (PolyData of the sphere)
    intersectionPolyDataFilter->SetInputData(0, Sphere->GetOutput());
    // Set the second PolyData as the surface of interest
    intersectionPolyDataFilter->SetInputData(1, Surface);
    // Update the filter
    intersectionPolyDataFilter->Update();
    std::cout<<"Update surface for intersection"<<std::endl;
}


/*------------------------------------------------------------------------*\
 * KeyPressInteractorNavigationStyle::OnKeyPress()
 * Event on key press
\*------------------------------------------------------------------------*/
void KeyPressInteractorNavigationStyle::OnKeyPress()
{
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Interactor;
    std::string key = rwi->GetKeySym();

    // Set the focal point
    Camera->SetDistance(1);

    // Handle UP arrow key
    if(key == "Up")
    {
        // Camera will look upward
        Camera->Elevation(-1);
    }

    // Handle Down arrow key
    if(key == "Down")
    {
        // Camera will look backward
        Camera->Elevation(1);
    }

    // Handle Left arrow key
    if(key == "Left")
    {
    Camera->Azimuth(1);
    }

    // Handle Right arrow key
    if(key == "Right")
    {
    Camera->Azimuth(-1);
    }

    // Handle z key: move forward
    if(key == "z" || key=="Z")
    {
        // Move forward
        Camera->Dolly(5);

        std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;

        // Set the bbox of the camera at the correct location
        Sphere->SetCenter(Camera->GetPosition());

        std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;

        // Update the IntersectionFilter with the new PolyData
        intersectionPolyDataFilter->Update();

        // Get the number of intersections
        nb_inter = intersectionPolyDataFilter->GetNumberOfIntersectionPoints();
        std::cout<<"Inters: "<<nb_inter<<std::endl;

        // If there are some intersections
        // Then move backward to get back to the old position
        // This method is a work-in-progress: it would be better to test the interaction at the new position with another camera object
        // and then move the real camera only if there are no intersection in the position
        if(nb_inter >= 1)
        {
            // Move backward
            Camera->Dolly(0.2);
            std::cout<<"don't move"<<std::endl;
        }


    }

    // Handle s key: move backward
    if(key == "s")
    {
        // Move backward
        Camera->Dolly(0.4);

        std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;

        // Set the bbox of the camera at the correct location
        Sphere->SetCenter(Camera->GetPosition());

        std::cout<<"sphere x:"<<Sphere->GetCenter()[0]<<" y:"<<Sphere->GetCenter()[1]<<" z:"<<Sphere->GetCenter()[2]<<std::endl;

        // Update the IntersectionFilter with the new PolyData
        intersectionPolyDataFilter->Update();

        // Get the number of intersections
        nb_inter = intersectionPolyDataFilter->GetNumberOfIntersectionPoints();
        std::cout<<"Inters: "<<nb_inter<<std::endl;

        // If there are some intersections
        // Then move forward to get back to the old position
        // This method is a work-in-progress: it would be better to test the interaction at the new position with another camera object
        // and then move the real camera only if there are no intersection in the position
        if(nb_inter >= 1)
        {
            // Move forward
            Camera->Dolly(10);
            std::cout<<"don't move"<<std::endl;
        }
    }

    // Handle escape key
    if(key == "Escape")
    {
        // Quit app
        exit(0);
    }

    // Set Clipping Range
    double dis[2]={0.5,1000.0};
    Camera->SetClippingRange(dis);

    // Set the focal point of the camera
    Camera->SetDistance(1);

    // Move the bbox of the camera accordingly with the camera
    Sphere->SetCenter(Camera->GetPosition());

    // Render everything
    this->Interactor->GetRenderWindow()->Render();

 // vtkInteractorStyleTrackballCamera::OnKeyPress();
}
