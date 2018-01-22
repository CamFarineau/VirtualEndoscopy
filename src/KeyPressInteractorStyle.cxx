/*
**    CPE Lyon
**    Copyright (C) 2018 Camille FARINEAU / Nicolas Ranc
**    Projet Majeur - Virtual Endoscopy
**
**    KeyPressInteractorStyle.cxx
**    Interactor Style for the Reslice Image Viewer : handle key event (to point a location for the camera of the 3D Viewer)
*/


#include "KeyPressInteractorStyle.h"

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::KeyPressInteractorStyle
 * Constructor
\*------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::KeyPressInteractorStyle
 * Destructor
\*------------------------------------------------------------------------*/
KeyPressInteractorStyle::~KeyPressInteractorStyle()
{
    this->Viewer     = NULL;
    this->Picker     = NULL;
    this->Annotation = NULL;
    this->Camera     = NULL;
    this->Interactor = NULL;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetPicker
 * Set the correct picker
 * Param: picker
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetPicker(vtkPropPicker *picker)
{
    this->Picker = picker;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetAnnotation
 * Set the correct corner annotation
 * Param: annotation
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetAnnotation(vtkCornerAnnotation *annotation)
{
    this->Annotation = annotation;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetInteractor
 * Set the correct interactor of this interactor style
 * Param: interactor
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& Interactor)
{
    this->Interactor=Interactor;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetViewer
 * Set the correct ResliceImageViewer with this interactor style
 * Param: viewer
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetViewer(vtkResliceImageViewer *viewer)
{
    this->Viewer = viewer;
}

/*****************************************************************************/
// All function and variables below are related to the 3D Viewer. It is useful here to directly place the camera at the correct location
// when the user pick a position in the ResliceImageViewer
// And it is useful to know the differents cells (Triangles) around the camera in the 3D Viewer (for the collision algorithm)

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetCamera
 * Set the camera of the 3D viewer (useful to put the camera at the same location the user picked)
 * Param: camera
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetCamera(const vtkSmartPointer<vtkCamera>& camera)
{
    this->Camera=camera;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetSurface
 * Set the surface of the object of intereset in the 3D Viewer
 * Param: surface
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetSurface(const vtkSmartPointer<vtkDecimatePro> &surface){
    Surface=surface;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetStyleNav
 * Set the interactor style of the interactor of the 3D Viewer (useful to interact with the 3D viewer directly from here)
 * Param: viewer
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetStyleNav(const vtkSmartPointer<KeyPressInteractorNavigationStyle>& styleNav){
    StyleNav=styleNav;
}

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::SetViewerNav
 * Set the RenderWindow of the 3D Viewer (useful to interact with the 3D viewer directly from here)
 * Param: viewer
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::SetViewerNav(const vtkSmartPointer<vtkRenderWindow>& renderWindow)
{
    this->ViewerNav = renderWindow;
}
/*****************************************************************************/

/*------------------------------------------------------------------------*\
 * KeyPressInteractorStyle::OnChar
 * Will be triggered if there is an event on a key
\*------------------------------------------------------------------------*/
void KeyPressInteractorStyle::OnChar()
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
            this->Annotation->SetText(0, "Off Image");
            interactor->Render();
            // Pass the event further on
            style->OnMouseMove();
            return;
        }

        // Get the coordinates of the pick inside the slices
        double pos[3];
        this->Picker->GetPickPosition(pos);

        // Get the orientation of the slice that the user picked from
        int orientation = this->Viewer->GetSliceOrientation();
        // Depending on the orientation get the correct coordinates
        switch (orientation)
        {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            coordonnees[0] = vtkMath::Round(pos[0]);
            coordonnees[1] = vtkMath::Round(pos[1]);
            coordonnees[2] = this->Viewer->GetSlice();
            break;
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
        default:;
        }

        // Set the position of the camera accordingly to the position picked by the user
        Camera->SetPosition(coordonnees[0],coordonnees[1],coordonnees[2]);

        // Set the clipping range to prevent issues
        double dis[2]={0.5,1000.0};
        Camera->SetClippingRange(dis);

        // Render the 3D Viewer
        ViewerNav->Render();

        std::cout<<"coord: "<<coordonnees[0]<<" , "<<coordonnees[1]<<" , "<<coordonnees[2]<<std::endl;

        /******************************************************/
        // This sample of code is in charge of create a surface from the cells (triangles) near the camera
        // This way it will look for collision between the bbox of the camera and the surface only with these triangles that are near the camera
        // and not all the triangles of the surface
        // It is freely inspirated from this code : https://www.vtk.org/Wiki/VTK/Examples/Cxx/PolyData/ExtractSelectionCells

        // Create a cell locator
        vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();

        // Set the data as the surface
        cellLocator->SetDataSet(Surface->GetOutput());

        // Create the cell locator
        cellLocator->BuildLocator();

        // Bounding box of the camera: cube where the camera is at the center
        // This code will look for all cells inside this cube
        double* bounds = new double[6];
        bounds[0] = coordonnees[0]-20.0;
        bounds[1] = coordonnees[0]+20.0;
        bounds[2] = coordonnees[1]-20.0;
        bounds[3] = coordonnees[1]+20.0;
        bounds[4] = coordonnees[2]-20.0;
        bounds[5] = coordonnees[2]+20.0;

        // Create a list of Ids
        vtkIdList* cellIdList = vtkIdList::New();

        // Find the cell of the surface inside the bbox of the camera
        // This function will populate the cellIdList
        cellLocator->FindCellsWithinBounds(bounds,cellIdList);

        std::cout<<"nb cell: "<<cellIdList->GetNumberOfIds()<<std::endl;

        // Create an array of Ids (needed to select the different cells, a list is not working)
        vtkSmartPointer<vtkIdTypeArray> cellIdArray = vtkSmartPointer<vtkIdTypeArray>::New();
        cellIdArray->SetNumberOfComponents(1);

        // Put all Ids we found in the list inside this array
        for(unsigned int i = 0; i < cellIdList->GetNumberOfIds(); i++)
        {
            cellIdArray->InsertNextValue(cellIdList->GetId(i));
        }

        // Create a selection node
        // This node will look for the cell with their Id
        vtkSmartPointer<vtkSelectionNode> node = vtkSmartPointer<vtkSelectionNode>::New();
        node->SetFieldType(vtkSelectionNode::CELL);
        node->SetContentType(vtkSelectionNode::INDICES);
        // Set the list of Ids to the node
        node->SetSelectionList(cellIdArray);

        // Create a selection object, pass the node to this object
        vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
        selection->AddNode(node);

        // Create an extractor to get the different cells
        vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
        // Set the input connection to the surface in question
        extractSelection->SetInputConnection(0, Surface->GetOutputPort());
        // The data are the selection of cells inside this surface
        extractSelection->SetInputData(1, selection);
        extractSelection->Update();

        // Create a grid that will contain all selected cells. It will have to create a PolyData from this gris
        vtkSmartPointer<vtkUnstructuredGrid> selectedCells = vtkSmartPointer<vtkUnstructuredGrid>::New();
        // Get all the cells that have been selected
        selectedCells->ShallowCopy(extractSelection->GetOutput());

        std::cout << "There are " << selectedCells->GetNumberOfPoints()
                  << " points in the selection." << std::endl;
        std::cout << "There are " << selectedCells->GetNumberOfCells()
                  << " cells in the selection." << std::endl;

        // Create a geometry filter to change the grid into a PolyData object
        vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
        // Set the grid as data
        geometryFilter->SetInputData(selectedCells);
        geometryFilter->Update();
        // Get the PolyData created from the grid
        nearestSurface = geometryFilter->GetOutput();

        // Set the surface for the collision as this new limited surface containing the nearest cells from the camera
        StyleNav->SetSurface(nearestSurface);
        // Update the InteractionFilter
        StyleNav->SetIntersectionPolyDataFilter();

        // Render everything
        this->Interactor->GetRenderWindow()->Render();

    }

}
