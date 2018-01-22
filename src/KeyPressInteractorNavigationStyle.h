#ifndef KEYPRESSINTERACTORNAVIGATIONSTYLE_H
#define KEYPRESSINTERACTORNAVIGATIONSTYLE_H

#include <vtkVersion.h>
#include <vtkAssemblyPath.h>
#include <vtkCell.h>
#include <vtkCommand.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageActor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPointData.h>
#include <vtkPropPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkImageNoiseSource.h>
#include <vtkImageCast.h>
#include <vtkMath.h>
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkResliceImageViewer.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
//#include "vtkTestUtilities.h"
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include "vtkPolyData.h"
#include "vtkCameraActor.h"
#include "vtkSelectEnclosedPoints.h"
#include "vtkIntersectionPolyDataFilter.h"
#include "vtkCellLocator.h"


class KeyPressInteractorNavigationStyle : public vtkInteractorStyleTrackballCamera
{
public:

    /**
     * @brief New instance of KeyPressInteractorNavigationStyle
     * @return an instance of KeyPressInteractorNavigationStyle
     */
    static KeyPressInteractorNavigationStyle* New();

    /**
     * @brief Destructor of KeyPressInteractorNavigationStyle
     * @return
     */
    ~KeyPressInteractorNavigationStyle();

    /**
     * @brief Constructor of KeyPressInteractorNavigationStyle
     */
    KeyPressInteractorNavigationStyle();

    /**
     * @brief Macro type of KeyPressInteractorNavigationStyle is a vtkInteractorStyleTrackballCamera
     */
    vtkTypeMacro(KeyPressInteractorNavigationStyle, vtkInteractorStyleTrackballCamera);

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
    void SetSurface(const vtkSmartPointer<vtkPolyData>& surface);

    /**
     * @brief Set a particular sphere representing the camera bounding box
     * @param sphere : a vtkSphereSource that will be used as a bbox
     */
    void SetSphere(const vtkSmartPointer<vtkSphereSource> &sphere);

    /**
     * @brief Set the 2 vtkPolyData for the Interaction Filter (bbox of the camera and the surface / object)
     */
    void SetInteractionPolyDataFilter();

    /**
     * @brief Event when a key is pressed
     */
    virtual void OnKeyPress();

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
     * @brief Surface: iso-surface created with marching cubes
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

};

#endif // KEYPRESSINTERACTORNAVIGATIONSTYLE_H
