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
#include "vtkTestUtilities.h"
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include "vtkResliceImageViewer.h"

using namespace std;


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



// The mouse motion callback, to pick the image and recover pixel values
class vtkImageInteractionCallback : public vtkCommand
{
public:

    static vtkImageInteractionCallback *New()
    {
        return new vtkImageInteractionCallback;
    }
  vtkImageInteractionCallback();
  ~vtkImageInteractionCallback();
  void SetPicker(vtkPropPicker *picker);
  void SetAnnotation(vtkCornerAnnotation *annotation);
  void SetViewer(vtkResliceImageViewer *viewer);
  virtual void Execute(vtkObject *, unsigned long vtkNotUsed(event), void *);

private:
  vtkResliceImageViewer*      Viewer;      // Pointer to the viewer
  vtkPropPicker*        Picker;      // Pointer to the picker
  vtkCornerAnnotation*  Annotation;  // Pointer to the annotation
};


#endif // vtkImageInteractionCallback_H
