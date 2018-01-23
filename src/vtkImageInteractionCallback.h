#ifndef vtkImageInteractionCallback_H
#define vtkImageInteractionCallback_H

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

#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
//#include "vtkTestUtilities.h"
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include "vtkResliceImageViewer.h"

using namespace std;

/*------------------------------------------------------------------------*\
Class vtkImageInteractionCallback

Manage mouse move interactor, display on 2D slices text (current cursor
inside or outside current viewer).

This code comes from this source code: https:
//www.vtk.org/Wiki/VTK/Examples/Cxx/Images/PickPixel2

\*------------------------------------------------------------------------*/


// Template for image value reading
template<typename T>
void vtkValueMessageTemplate(vtkImageData* image, int* position,
                             std::string& message)
{
  T* tuple = ((T*)image->GetScalarPointer(position));
  int components = image->GetNumberOfScalarComponents();
  for (int c = 0; c < components; ++c)
    {
    message += vtkVariant(tuple[c]).ToString();
    if (c != (components - 1))
      {
      message += ", ";
      }
    }
  message += " )";
}


/**
 * @brief The vtkImageInteractionCallback class: Class used to return mouse coordinates from 2D to 3D world
 */
class vtkImageInteractionCallback : public vtkCommand
{
public:

    /**
     * @brief Dynamic allocation of vtkImageInteractionCallback
     * @return an instance of vtkImageInteractionCallback
     */
    static vtkImageInteractionCallback *New()
    {
        return new vtkImageInteractionCallback;
    }

   /**
   * @brief Constructor
   */
  vtkImageInteractionCallback();

  /**
   * @brief Destructor
   */
  ~vtkImageInteractionCallback();

  /**
   * @brief SetPicker : Set the correct picker (to know where the user clicked)
   * @param picker
   */
  void SetPicker(vtkPropPicker *picker);

  /**
   * @brief SetAnnotation : Set the correct annotation (text displayed on the vtkResliceImageViewer)
   * @param annotation
   */
  void SetAnnotation(vtkCornerAnnotation *annotation);

  /**
   * @brief SetViewer : Set the correct viewer resliced
   * @param viewer
   */
  void SetViewer(vtkResliceImageViewer *viewer);

  /**
   * @brief Execute : Similar to the callback function
   * @param vtkObject * : Current object
   * @param event : Id of the event
   */
  virtual void Execute(vtkObject *, unsigned long vtkNotUsed(event), void *);

private:
  /**
   * @brief Viewer: the ResliceImageViewer associated with this InteractorStyle
   */
  vtkResliceImageViewer*      Viewer;

  /**
   * @brief Picker: the Picker associated with this InteractorStyle
   */
  vtkPropPicker*        Picker;

  /**
   * @brief Annotation: the Corner Annotation associated with this InteractorStyle
   */
  vtkCornerAnnotation*  Annotation;
};


#endif // vtkImageInteractionCallback_H
