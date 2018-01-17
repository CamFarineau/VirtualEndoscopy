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

#include "vtkDICOMImageReader.h"

#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkResliceImageViewer.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTestUtilities.h"
#include <vtkActor.h>
#include <vtkRenderWindow.h>


class KeyPressInteractorNavigationStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static KeyPressInteractorNavigationStyle* New();

    ~KeyPressInteractorNavigationStyle();
    KeyPressInteractorNavigationStyle();
    vtkTypeMacro(KeyPressInteractorNavigationStyle, vtkInteractorStyleTrackballCamera);

    void SetCamera(const vtkSmartPointer<vtkCamera>& camera_);
    void SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& interactor);

    virtual void OnKeyPress();

  private:
     vtkSmartPointer<vtkCamera> camera;
     vtkSmartPointer<vtkRenderWindowInteractor> Interactor;

};

#endif // KEYPRESSINTERACTORNAVIGATIONSTYLE_H
