/*
**    CPE Lyon
**    2018 Camille FARINEAU / Nicolas Ranc
**    Projet Majeur - Virtual Endoscopy
**
**    SliceViewerKeyPressInteractorStyle.cxx
**    Interactor Style for the Reslice Image Viewer : handle key event (to point a location for the camera of the 3D Viewer)
**    It will be used each time a key is pressed when the current viewer is one the Slice Viewer
*/


#include "SliceViewerKeyPressInteractorStyle.h"

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::SliceViewerKeyPressInteractorStyle
 * Constructor
\*------------------------------------------------------------------------*/
SliceViewerKeyPressInteractorStyle::SliceViewerKeyPressInteractorStyle()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Camera     = NULL;
    this->Interactor = NULL;
    coordonnees[0] = 0;
    coordonnees[1] = 0;
    coordonnees[2] = 0;
}

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::SliceViewerKeyPressInteractorStyle
 * Destructor
\*------------------------------------------------------------------------*/
SliceViewerKeyPressInteractorStyle::~SliceViewerKeyPressInteractorStyle()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Camera     = NULL;
    this->Interactor = NULL;
}

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::SetPicker
 * Set the correct picker
 * Param: picker
\*------------------------------------------------------------------------*/
void SliceViewerKeyPressInteractorStyle::SetPicker(vtkPropPicker *picker)
{
    this->Picker = picker;
}

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::SetInteractor
 * Set the correct interactor of this interactor style
 * Param: interactor
\*------------------------------------------------------------------------*/
void SliceViewerKeyPressInteractorStyle::SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& Interactor)
{
    this->Interactor=Interactor;
}

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::SetViewer
 * Set the correct ResliceImageViewer with this interactor style
 * Param: viewer
\*------------------------------------------------------------------------*/
void SliceViewerKeyPressInteractorStyle::SetViewer(vtkResliceImageViewer *viewer)
{
    this->Viewer = viewer;
}

/*****************************************************************************/
// All function and variables below are related to the 3D Viewer. It is useful here to directly place the camera at the correct location
// when the user pick a position in the ResliceImageViewer
// And it is useful to know the differents cells (Triangles) around the camera in the 3D Viewer (for the collision algorithm)

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::SetCamera
 * Set the camera of the 3D viewer (useful to put the camera at the same location the user picked)
 * Param: camera
\*------------------------------------------------------------------------*/
void SliceViewerKeyPressInteractorStyle::SetCamera(const vtkSmartPointer<vtkCamera>& camera)
{
    this->Camera=camera;
}

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::SetViewerNav
 * Set the RenderWindow of the 3D Viewer (useful to interact with the 3D viewer directly from here)
 * Param: viewer
\*------------------------------------------------------------------------*/
void SliceViewerKeyPressInteractorStyle::SetViewerNav(const vtkSmartPointer<vtkRenderWindow>& renderWindow)
{
    this->ViewerNav = renderWindow;
}
/*****************************************************************************/

/*------------------------------------------------------------------------*\
 * SliceViewerKeyPressInteractorStyle::OnChar
 * Will be triggered if there is an event on a key
\*------------------------------------------------------------------------*/
void SliceViewerKeyPressInteractorStyle::OnChar()
{
    // Get the keypress: need the renderWindowInteractor of the Viewer that was used
    vtkRenderWindowInteractor *interactor = this->Viewer->GetRenderWindow()->GetInteractor();
    std::string key = interactor->GetKeySym();


    // If "p" was pressed
    if(key == "P" || key == "p")
    {
        // Reset the coordinates
        coordonnees[0] = 0;
        coordonnees[1] = 0;
        coordonnees[2] = 0;

        // Get the renderer
        vtkRenderer* renderer = this->Viewer->GetRenderer();
        // Get the image actor
        vtkImageActor* actor = this->Viewer->GetImageActor();
        // Get the style of the interactor
        vtkInteractorStyleImage *style = vtkInteractorStyleImage::SafeDownCast(interactor->GetInteractorStyle());


        // GetEventPosition is the location of the mouse
        // We need to pick at this location to find the location in the slices (which slices of each orientation is at this pick)
        this->Picker->Pick(interactor->GetEventPosition()[0],interactor->GetEventPosition()[1],0.0, renderer);

        // Verification that we pick something inside the slice and not outside
        // This sample of code comes from this source code: https://www.vtk.org/Wiki/VTK/Examples/Cxx/Images/PickPixel2

        // Get path of the Picker
        vtkAssemblyPath* path = this->Picker->GetPath();
        bool validPick = false;
        // Path exist
        if (path)
        {
            // Create an iterator to get through list of node
            vtkCollectionSimpleIterator sit;
            path->InitTraversal(sit);
            vtkAssemblyNode *node;
            // Go through all node
            for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
            {
                node = path->GetNextNode(sit);
                // If the actor is the same
                // Then this is a valid pick (this is the actor from the slice)
                if (actor == vtkImageActor::SafeDownCast(node->GetViewProp()))
                {
                    validPick = true;
                }
            }
        }

        // If this is not a valid pick (outside the slice)
        // Then prompt "Off Image" and return
        if (!validPick)
        {
            interactor->Render();
            // Pass the event further on
            style->OnMouseMove();
            return;
        }

        // Get the coordinates of the pick inside the slices
        double position[3];
        this->Picker->GetPickPosition(position);

        // Get the orientation of the slice that the user picked from
        int orientation = this->Viewer->GetSliceOrientation();
        // Depending on the orientation get the correct coordinates
        switch (orientation)
        {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            coordonnees[0] = vtkMath::Round(position[0]);
            coordonnees[1] = vtkMath::Round(position[1]);
            coordonnees[2] = this->Viewer->GetSlice();
            break;
        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            coordonnees[0] = vtkMath::Round(position[0]);
            coordonnees[1] = this->Viewer->GetSlice();
            coordonnees[2] = vtkMath::Round(position[2]);
            break;
        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            coordonnees[0] = this->Viewer->GetSlice();
            coordonnees[1] = vtkMath::Round(position[1]);
            coordonnees[2] = vtkMath::Round(position[2]);
            break;
        default:;
        }

        // Set the position of the camera accordingly to the position picked by the user
        Camera->SetPosition(coordonnees[0],coordonnees[1],coordonnees[2]);

        // Set the clipping range to prevent issues
        double dis[2]={0.5,1000.0};
        Camera->SetClippingRange(dis);

        // Render the 3D Viewer
        ViewerNav->Render();

        //std::cout<<"coord: "<<coordonnees[0]<<" , "<<coordonnees[1]<<" , "<<coordonnees[2]<<std::endl;

        // Render everything
        this->Interactor->GetRenderWindow()->Render();

    }

}
