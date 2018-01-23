/*
**    CPE Lyon
**    2018 Camille FARINEAU / Nicolas Ranc
**    Projet Majeur - Virtual Endoscopy
**
**    KeyPressInteractorStyle.h
**    Interactor Style for the Reslice Image Viewer : handle key event (to point a location for the camera of the 3D Viewer)
*/

#ifndef KeyPressInteractorStyle_H
#define KeyPressInteractorStyle_H

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
#include "vtkDICOMImageReader.h"
#include "vtkCellLocator.h"
#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkResliceImageViewer.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include "vtkSelectionNode.h"
#include "vtkPolyData.h"
#include "vtkSelection.h"
#include "vtkExtractSelection.h"
#include "vtkUnstructuredGrid.h"
#include "vtkGeometryFilter.h"
#include "vtkDecimatePro.h"
#include "KeyPressInteractorNavigationStyle.h"


/**
 * @brief The KeyPressInteractorStyle class: class used to interact with the ResliceImageViewer
 * (pick a position where the camera will be placed in the 3D viewer)
 */
class KeyPressInteractorStyle : public vtkInteractorStyleImage
{
public:

    /**
     * @brief Constructor
     */
    KeyPressInteractorStyle();

    /**
     * @brief Destructor
     */
    ~KeyPressInteractorStyle();

    /**
     * @brief vtkTypeMacro: KeyPressInteractorStyle is a type vtkInteractorStyleImage
     */
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleImage);

    /**
     * @brief Dynamic allocation of KeyPressInteractorStyle
     * @return  an instance of KeyPressInteractorStyle
     */
    static KeyPressInteractorStyle* New();


    /**
     * @brief Set the correct picker (to know where the user clicked)
     * @param picker
     */
    void SetPicker(vtkPropPicker *picker);

    /**
     * @brief Set the correct ResliceImageViewer for this interactor style
     * @param viewer
     */
    void SetViewer(vtkResliceImageViewer *viewer);

    /**
     * @brief Set the correct Interactor that owns this style
     * @param interactor
     */
    void SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& interactor);

    /**
     * @brief Event when a key of the keyboard is pressed
     */
    virtual void OnChar();

    /**
     * @brief nearestSurface: every cells (Triangles) near the camera (inside a bbox cube around the camera) will be put in this PolyData
     */
    vtkSmartPointer<vtkPolyData> nearestSurface;



    // All function and variables below are related to the 3D Viewer. It is useful here to directly place the camera at the correct location
    // when the user pick a position in the ResliceImageViewer
    // And it is useful to know the differents cells (Triangles) around the camera in the 3D Viewer (for the collision algorithm)
    /**
     * @brief Set the camera of the 3D viewer
     * @param camera
     */
    void SetCamera(const vtkSmartPointer<vtkCamera>& camera);

    /**
     * @brief Set the RenderWindow of the 3D Viewer
     * @param renderWindow
     */
    void SetViewerNav(const vtkSmartPointer<vtkRenderWindow>& renderWindow);

    /**
     * @brief Set the InteractorStyle of the interactor of the 3D Viewer
     * @param styleNav
     */
    void SetStyleNav(const vtkSmartPointer<KeyPressInteractorNavigationStyle>& styleNav);
    /**
     * @brief StyleNav: InteractorStyle of the 3D Viewer (useful to interact with the 3D Viewer directly from here)
     */
    vtkSmartPointer<KeyPressInteractorNavigationStyle> StyleNav;

    /**
     * @brief coordonnees: position where the camera will be placed (position pick by the user)
     */
    int coordonnees[3];


private:

    /**
     * @brief Viewer: the ResliceImageViewer associated with this InteractorStyle
     */
    vtkSmartPointer<vtkResliceImageViewer> Viewer;

    /**
     * @brief Picker: the Picker associated with this InteractorStyle
     */
    vtkSmartPointer<vtkPropPicker> Picker;

    /**
     * @brief Interactor: the interactor associated with this InteractorStyle
     */
    vtkSmartPointer<vtkRenderWindowInteractor> Interactor;

    // All variables below are related to the 3D Viewer. It is useful here to directly place the camera at the correct location
    // when the user pick a position in the ResliceImageViewer
    // And it is useful to know the differents cells (Triangles) around the camera in the 3D Viewer (for the collision algorithm)
    /**
     * @brief Camera: the Camera of the 3D Viewer
     */
    vtkSmartPointer<vtkCamera> Camera;

    /**
     * @brief ViewerNav: the RenderWindow of the 3D Viewer
     */
    vtkSmartPointer<vtkRenderWindow> ViewerNav;

};

#endif // KeyPressInteractorStyle_H
