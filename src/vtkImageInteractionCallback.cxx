#include "vtkImageInteractionCallback.h"



vtkImageInteractionCallback::vtkImageInteractionCallback()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Annotation = NULL;
}

vtkImageInteractionCallback::~vtkImageInteractionCallback()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Annotation = NULL;
}

void vtkImageInteractionCallback::SetPicker(vtkPropPicker *picker)
{
    this->Picker = picker;
}

void vtkImageInteractionCallback::SetAnnotation(vtkCornerAnnotation *annotation)
{
    this->Annotation = annotation;
}

void vtkImageInteractionCallback::SetViewer(vtkImageViewer2 *viewer)
{
    this->Viewer = viewer;
}

void vtkImageInteractionCallback::Execute(vtkObject *, unsigned long vtkNotUsed(event), void *)
{
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


    int image_coordinate[3];

    int axis = this->Viewer->GetSliceOrientation();
    switch (axis)
    {
    case vtkResliceImageViewer::SLICE_ORIENTATION_XZ:
        image_coordinate[0] = vtkMath::Round(pos[0]);
        image_coordinate[1] = this->Viewer->GetSlice();
        image_coordinate[2] = vtkMath::Round(pos[2]);
        break;
    case vtkResliceImageViewer::SLICE_ORIENTATION_YZ:
        image_coordinate[0] = this->Viewer->GetSlice();
        image_coordinate[1] = vtkMath::Round(pos[1]);
        image_coordinate[2] = vtkMath::Round(pos[2]);
        break;
    default:  // vtkResliceImageViewer::SLICE_ORIENTATION_XY
        image_coordinate[0] = vtkMath::Round(pos[0]);
        image_coordinate[1] = vtkMath::Round(pos[1]);
        image_coordinate[2] = this->Viewer->GetSlice();
        break;
    }


    std::string message = "Location: ( ";
    message += vtkVariant(image_coordinate[0]).ToString();
    message += ", ";
    message += vtkVariant(image_coordinate[1]).ToString();
    message += ", ";
    message += vtkVariant(image_coordinate[2]).ToString();
    message += " )\nValue: ( ";

    switch (image->GetScalarType())
    {
    vtkTemplateMacro((vtkValueMessageTemplate<VTK_TT>(image,
                                                      image_coordinate,
                                                      message)));

    default:
        return;
    }

    this->Annotation->SetText( 0, message.c_str() );
    interactor->Render();
    style->OnMouseMove();
}
