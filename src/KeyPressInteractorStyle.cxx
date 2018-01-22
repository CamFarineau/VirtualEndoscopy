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

void KeyPressInteractorStyle::SetSurface(const vtkSmartPointer<vtkDecimatePro> &surface){
    Surface=surface;
}

void KeyPressInteractorStyle::SetStyleNav(const vtkSmartPointer<KeyPressInteractorNavigationStyle>& styleNav){
    StyleNav=styleNav;
}

void KeyPressInteractorStyle::SetViewerNav(const vtkSmartPointer<vtkRenderWindow>& viewer)
{
    this->ViewerNav = viewer;
}


void KeyPressInteractorStyle::OnChar()
{
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Viewer->GetRenderWindow()->GetInteractor();

    std::cout<<"KeyPressInteractorStyle"<<std::endl;
    std::string key = rwi->GetKeySym();
    std::cout<<"key "<<key<<std::endl;


    // Output the key that was pressed
    if(key == "P" || key == "p")
    {
        //rwi->EnableRenderOn();

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
        double dis[2]={0.5,1000.0};
        Camera->SetClippingRange(dis);
        ViewerNav->Render();

        //surfaceRenderer->ResetCameraClippingRange();
        std::cout<<"coord: "<<coordonnees[0]<<" , "<<coordonnees[1]<<" , "<<coordonnees[2]<<std::endl;


        vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
          cellLocator->SetDataSet(Surface->GetOutput());
          cellLocator->BuildLocator();
          double* bounds = new double[6];
          bounds[0] = coordonnees[0]-20.0;
          bounds[1] = coordonnees[0]+20.0;
          bounds[2] = coordonnees[1]-20.0;
          bounds[3] = coordonnees[1]+20.0;
          bounds[4] = coordonnees[2]-20.0;
          bounds[5] = coordonnees[2]+20.0;

          vtkIdList* cellId = vtkIdList::New(); //the cell id of the cell containing the closest point will be returned here
          cellLocator->FindCellsWithinBounds(bounds,cellId);
          std::cout<<"nb cell: "<<cellId->GetNumberOfIds()<<std::endl;

        vtkSmartPointer<vtkIdTypeArray> ids =
            vtkSmartPointer<vtkIdTypeArray>::New();
        ids->SetNumberOfComponents(1);

        // Specify that we want to extract cells 10 through 19
        for(unsigned int i = 0; i < cellId->GetNumberOfIds(); i++)
          {
          ids->InsertNextValue(cellId->GetId(i));
          }

        vtkSmartPointer<vtkSelectionNode> selectionNode =
          vtkSmartPointer<vtkSelectionNode>::New();
        selectionNode->SetFieldType(vtkSelectionNode::CELL);
        selectionNode->SetContentType(vtkSelectionNode::INDICES);
        selectionNode->SetSelectionList(ids);


        vtkSmartPointer<vtkSelection> selection =
          vtkSmartPointer<vtkSelection>::New();
        selection->AddNode(selectionNode);

        vtkSmartPointer<vtkExtractSelection> extractSelection =
          vtkSmartPointer<vtkExtractSelection>::New();
        extractSelection->SetInputConnection(0, Surface->GetOutputPort());

        extractSelection->SetInputData(1, selection);
        extractSelection->Update();

        // In selection
        vtkSmartPointer<vtkUnstructuredGrid> selected =
          vtkSmartPointer<vtkUnstructuredGrid>::New();
        selected->ShallowCopy(extractSelection->GetOutput());

        std::cout << "There are " << selected->GetNumberOfPoints()
                  << " points in the selection." << std::endl;
        std::cout << "There are " << selected->GetNumberOfCells()
                  << " cells in the selection." << std::endl;

        vtkSmartPointer<vtkGeometryFilter> geometryFilter =
            vtkSmartPointer<vtkGeometryFilter>::New();
         geometryFilter->SetInputData(selected);
         geometryFilter->Update();
         closedSurface = geometryFilter->GetOutput();

        StyleNav->SetSurface(closedSurface);
        StyleNav->SetInteractionPolyDataFilter();

        this->Interactor->GetRenderWindow()->Render();

        //interactor->Render();
    }
    //rwi->EnableRenderOff();



    //KeyPressInteractorStyle::OnKeyPress();

}
