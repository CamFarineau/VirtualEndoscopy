/*
**    CPE Lyon
**    2018 Camille FARINEAU / Nicolas Ranc
**    Projet Majeur - Virtual Endoscopy
**
**    SurfaceViewerKeyPressInteractorStyle.h
**    Interactor Style for the 3D Viewer : handle key event (to move the camera in the scene)
*/


#ifndef SurfaceViewerKeyPressInteractorStyle_H
#define SurfaceViewerKeyPressInteractorStyle_H

#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkImageData.h"
#include "vtkCamera.h"
#include <vtkRenderWindow.h>
#include "vtkPolyData.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkIntersectionPolyDataFilter.h"
#include "vtkCellLocator.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkExtractSelection.h"
#include "vtkUnstructuredGrid.h"
#include "vtkGeometryFilter.h"
#include "vtkInteractorStyleTrackballCamera.h"

/**
 * @brief The SurfaceViewerKeyPressInteractorStyle class: class used to interact with the 3D Viewer with the keyboard (move the camera in the scene)
 */
class SurfaceViewerKeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:

    /**
     * @brief New instance of SurfaceViewerKeyPressInteractorStyle
     * @return an instance of SurfaceViewerKeyPressInteractorStyle
     */
    static SurfaceViewerKeyPressInteractorStyle* New();

    /**
     * @brief Destructor of SurfaceViewerKeyPressInteractorStyle
     * @return
     */
    ~SurfaceViewerKeyPressInteractorStyle();

    /**
     * @brief Constructor of SurfaceViewerKeyPressInteractorStyle
     */
    SurfaceViewerKeyPressInteractorStyle();

    /**
     * @brief Macro type of SurfaceViewerKeyPressInteractorStyle is a vtkInteractorStyleTrackballCamera
     */
    vtkTypeMacro(SurfaceViewerKeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

    /**
     * @brief Function to set the camera of 3D viewer
     * @param camera_ : an instance of a camera object
     */
    void SetCamera(const vtkSmartPointer<vtkCamera>& camera_);

    /**
     * @brief Set a particular RenderWindowInteractor to this KeyPressInteractor (useful to move when a key is pressed)
     * @param interactor : a vtkRenderWindowInteractor of the view to be modified
     */
    void SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& interactor);

    /**
     * @brief Set a particular iso-surface to interact with
     * @param surface: a particular vtkPolyData representing the surface
     */
    void SetSurface(const vtkSmartPointer<vtkPolyData> &surface);

    /**
     * @brief Set a particular iso-surface to interact with, specific for collision
     * @param surface_col : a particular vtkPolyDataAlgorithm representing the surface
     */
    void SetSurfaceCollision(const vtkSmartPointer<vtkPolyDataAlgorithm> &surface_col);


    /**
     * @brief Set a particular sphere representing the camera bounding box
     * @param sphere : a vtkSphereSource that will be used as a bbox
     */
    void SetSphere(const vtkSmartPointer<vtkSphereSource> &sphere);

    /**
     * @brief Set the 2 vtkPolyData for the Interaction Filter (bbox of the camera and the surface / object)
     */
    void SetIntersectionPolyDataFilter();

    /**
     * @brief Event when a key is pressed
     */
    virtual void OnKeyPress();

    /**
     * @brief nearestSurface: every cells (Triangles) near the camera (inside a bbox cube around the camera) will be put in this PolyData
     */
    vtkSmartPointer<vtkPolyData> nearestSurface;

private:
    /**
     * @brief camera: a vtkCamera for the scene
     */
    vtkSmartPointer<vtkCamera> Camera;

    /**
     * @brief Interactor: a vtkRenderWindowInteractor to interact with the RenderWindow of the 3D scene
     */
    vtkSmartPointer<vtkRenderWindowInteractor> Interactor;

    /**
     * @brief Surface_col: PolyDataAlgorithm Object containing a PolyData of the surface for the collision
     */
    vtkSmartPointer<vtkPolyDataAlgorithm> Surface_col;

    /**
     * @brief Surface: PolyData of the surface of interest
     */
    vtkSmartPointer<vtkPolyData> Surface;

    /**
     * @brief Sphere: will be used as a bbox for the camera (collision between camera and surface)
     */
    vtkSmartPointer<vtkSphereSource> Sphere;

    /**
     * @brief intersectionPolyDataFilter: Intersection filter to know if there is an intersection between the bbox of the camera and the surface (collider)
     */
    vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionPolyDataFilter;

    /**
     * @brief nb_inter: Number of intersections between the bbxo of the camera and the surface
     */
    int nb_inter;

    /**
     * @brief cellIdArray: Create an array of Ids (needed to select the different cells, a list is not working)
     */
    vtkSmartPointer<vtkIdTypeArray> cellIdArray;

    /**
     * @brief node: Create a selection node. This node will look for the cell with their Id
     */
    vtkSmartPointer<vtkSelectionNode> node;

    /**
     * @brief selection: Create a selection object, pass the node to this object
     */
    vtkSmartPointer<vtkSelection> selection;

    /**
     * @brief extractSelection: Create an extractor to get the different cells
     */
    vtkSmartPointer<vtkExtractSelection> extractSelection;

    /**
     * @brief selectedCells: Create a grid that will contain all selected cells. It will have to create a PolyData from this gris
     */
    vtkSmartPointer<vtkUnstructuredGrid> selectedCells;

    /**
     * @brief geometryFilter: Create a geometry filter to change the grid into a PolyData object
     */
    vtkSmartPointer<vtkGeometryFilter> geometryFilter;

    /**
     * @brief collision: Enable/Disable collision between camera (sphere) and 3D model
     */
    bool collision;

};

#endif // SurfaceViewerKeyPressInteractorStyle_H
