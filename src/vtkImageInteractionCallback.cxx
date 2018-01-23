#include "vtkImageInteractionCallback.h"


/*------------------------------------------------------------------------*\
 * vtkImageInteractionCallback::vtkImageInteractionCallback
 * Constructor
\*------------------------------------------------------------------------*/
vtkImageInteractionCallback::vtkImageInteractionCallback()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Annotation = NULL;
}


/*------------------------------------------------------------------------*\
vtkImageInteractionCallback::~vtkImageInteractionCallback
 * Destructor
\*------------------------------------------------------------------------*/
vtkImageInteractionCallback::~vtkImageInteractionCallback()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Annotation = NULL;
}


/*------------------------------------------------------------------------*\
void vtkImageInteractionCallback::SetPicker
 * Param: picker : user click
\*------------------------------------------------------------------------*/
void vtkImageInteractionCallback::SetPicker(vtkPropPicker *picker)
{
    this->Picker = picker;
}


/*------------------------------------------------------------------------*\
void vtkImageInteractionCallback::SetAnnotation
 * Param: annotation : text on the reslice image
\*------------------------------------------------------------------------*/
void vtkImageInteractionCallback::SetAnnotation(vtkCornerAnnotation *annotation)
{
    this->Annotation = annotation;
}



/*------------------------------------------------------------------------*\
void vtkImageInteractionCallback::SetViewer
 * Param: viewer : current image
\*------------------------------------------------------------------------*/
void vtkImageInteractionCallback::SetViewer(vtkResliceImageViewer *viewer)
{
    this->Viewer = viewer;
}


/*------------------------------------------------------------------------*\
void vtkImageInteractionCallback::Execute
\*------------------------------------------------------------------------*/
void vtkImageInteractionCallback::Execute(vtkObject *, unsigned long vtkNotUsed(event), void *)
{
    //Catch elements corresponding to the current viewer
    vtkRenderWindowInteractor *interactor = this->Viewer->GetRenderWindow()->GetInteractor();
    vtkRenderer* renderer = this->Viewer->GetRenderer();
    vtkImageActor* actor = this->Viewer->GetImageActor();
    vtkImageData* image = this->Viewer->GetInput();
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());

#if VTK_MAJOR_VERSION <= 5
    image->Update();
#endif

    // Pick at the mouse location provided by the interactor
    this->Picker->Pick(interactor->GetEventPosition()[0],
            interactor->GetEventPosition()[1],
            0.0, renderer);

    // Verification that we pick something inside the slice and not outside
    // This sample of code comes from this source code: https://www.vtk.org/Wiki/VTK/Examples/Cxx/Images/PickPixel2

    // There could be other props assigned to this picker, so
    // make sure we picked the image actor
    vtkAssemblyPath* path = this->Picker->GetPath();
    bool validPick = false;

    // Verification that we pick something inside the slice and not outside
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
        this->Annotation->SetText(0, "Off Image");
        interactor->Render();
        // Pass the event further on
        style->OnMouseMove();
        return;
    }

    int coordinate[3];

    // Get the coordinates of the pick inside the slices
    double position[3];
    this->Picker->GetPickPosition(position);

    // Get the orientation of the slice that the user picked from
    int orientation = this->Viewer->GetSliceOrientation();
    // Depending on the orientation get the correct coordinates
    switch (orientation)
    {
    case vtkImageViewer2::SLICE_ORIENTATION_XY:
        coordinate[0] = vtkMath::Round(position[0]);
        coordinate[1] = vtkMath::Round(position[1]);
        coordinate[2] = this->Viewer->GetSlice();
        break;
    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
        coordinate[0] = vtkMath::Round(position[0]);
        coordinate[1] = this->Viewer->GetSlice();
        coordinate[2] = vtkMath::Round(position[2]);
        break;
    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
        coordinate[0] = this->Viewer->GetSlice();
        coordinate[1] = vtkMath::Round(position[1]);
        coordinate[2] = vtkMath::Round(position[2]);
        break;
    default:;
    }

    //Text displayed on the window
    std::string message = "Location: ( ";
    message += vtkVariant(coordinate[0]).ToString();
    message += ", ";
    message += vtkVariant(coordinate[1]).ToString();
    message += ", ";
    message += vtkVariant(coordinate[2]).ToString();
    message += " )\nValue: ( ";

    switch (image->GetScalarType())
    {
    vtkTemplateMacro((vtkValueMessageTemplate<VTK_TT>(image,
                                                      coordinate,
                                                      message)));

    default:
        return;
    }
    // Set the correct annotation
    this->Annotation->SetText( 0, message.c_str() );

    //Render the interactor
    interactor->Render();

    //Event binding
    style->OnMouseMove();
}
