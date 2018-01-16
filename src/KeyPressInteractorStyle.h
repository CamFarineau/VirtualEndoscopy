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

#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkResliceImageViewer.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTestUtilities.h"
#include <vtkActor.h>
#include <vtkRenderWindow.h>


// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleImage
{
  public:
    KeyPressInteractorStyle();
    ~KeyPressInteractorStyle();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleImage);
    int coordonnees[3];
    static KeyPressInteractorStyle* New();

    void SetPicker(vtkPropPicker *picker);
    void SetAnnotation(vtkCornerAnnotation *annotation);
    void SetViewer(vtkResliceImageViewer *viewer);
    virtual void OnKeyPress();

private:
  vtkResliceImageViewer*    Viewer;      // Pointer to the viewer
  vtkPropPicker*        Picker;      // Pointer to the picker
  vtkCornerAnnotation*  Annotation;  // Pointer to the annotation
};




#endif // KeyPressInteractorStyle_H
