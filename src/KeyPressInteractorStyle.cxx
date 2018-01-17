#include "KeyPressInteractorStyle.h"


KeyPressInteractorStyle::KeyPressInteractorStyle()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Annotation = NULL;
    this->Camera     = NULL;
    this->Interactor = NULL;
    coordonnees[0] = 0;
    coordonnees[1] = 0;
    coordonnees[2] = 0;
}

KeyPressInteractorStyle::~KeyPressInteractorStyle()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Annotation = NULL;
    this->Camera     = NULL;
    this->Interactor = NULL;
}

void KeyPressInteractorStyle::SetPicker(vtkPropPicker *picker)
{
    this->Picker = picker;
}

void KeyPressInteractorStyle::SetAnnotation(vtkCornerAnnotation *annotation)
{
    this->Annotation = annotation;
}

void KeyPressInteractorStyle::SetViewer(vtkResliceImageViewer *viewer)
{
    this->Viewer = viewer;
}

void KeyPressInteractorStyle::SetCamera(const vtkSmartPointer<vtkCamera>& camera)
{
    this->Camera=camera;
}

void KeyPressInteractorStyle::SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& Interactor)
{
    this->Interactor=Interactor;
}

void KeyPressInteractorStyle::OnKeyPress()
{
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Viewer->GetRenderWindow()->GetInteractor();
    std::cout<<"KeyPressInteractorStyle"<<std::endl;
    std::string key = rwi->GetKeySym();
    std::cout<<"key"<<key<<std::endl;

    // Output the key that was pressed
    if(key == "P" || key == "p")
    {

        std::cout<<"level: "<<Viewer->GetColorLevel()<<" ,window: "<<Viewer->GetColorWindow()<<std::endl;
        coordonnees[0] = 0;
        coordonnees[1] = 0;
        coordonnees[2] = 0;

        vtkRenderWindowInteractor *interactor =
                this->Viewer->GetRenderWindow()->GetInteractor();
        vtkRenderer* renderer = this->Viewer->GetRenderer();
        vtkImageActor* actor = this->Viewer->GetImageActor();
        vtkInteractorStyleImage *style = vtkInteractorStyleImage::SafeDownCast(
                    interactor->GetInteractorStyle());

        #if VTK_MAJOR_VERSION <= 5
                image->Update();
        #endif


        //Pick at the mouse location provided by the interactor
        this->Picker->Pick(interactor->GetEventPosition()[0],
                interactor->GetEventPosition()[1],
                0.0, renderer);

        // There could be other props assigned to this picker, so
        // make sure we picked the image actor
        vtkAssemblyPath* path = this->Picker->GetPath();
        bool validPick = false;

        if (path)
        {
            vtkCollectionSimpleIterator sit;
            path->InitTraversal(sit);
            vtkAssemblyNode *node;
            for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
            {
                node = path->GetNextNode(sit);
                if (actor == vtkImageActor::SafeDownCast(node->GetViewProp()))
                {
                    validPick = true;
                }
            }
        }

        if (!validPick)
        {
            this->Annotation->SetText(0, "Off Image");
            interactor->Render();
            // Pass the event further on
            style->OnMouseMove();
            return;
        }

        // Get the world coordinates of the pick
        double pos[3];
        this->Picker->GetPickPosition(pos);


        int axis = this->Viewer->GetSliceOrientation();
        switch (axis)
        {
        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            coordonnees[0] = vtkMath::Round(pos[0]);
            coordonnees[1] = this->Viewer->GetSlice();
            coordonnees[2] = vtkMath::Round(pos[2]);
            break;
        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            coordonnees[0] = this->Viewer->GetSlice();
            coordonnees[1] = vtkMath::Round(pos[1]);
            coordonnees[2] = vtkMath::Round(pos[2]);
            break;
        default:  // vtkImageViewer2::SLICE_ORIENTATION_XY
            coordonnees[0] = vtkMath::Round(pos[0]);
            coordonnees[1] = vtkMath::Round(pos[1]);
            coordonnees[2] = this->Viewer->GetSlice();
            break;
        }
        Camera->SetPosition(coordonnees[0],coordonnees[1],coordonnees[2]);
        //surfaceRenderer->ResetCameraClippingRange();

        this->Interactor->GetRenderWindow()->Render();

        //interactor->Render();
    }
    vtkInteractorStyleImage::OnKeyPress();

}
