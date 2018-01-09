#ifndef QtVTKRenderWindows_H
#define QtVTKRenderWindows_H

#include "vtkSmartPointer.h"
#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDistanceWidget.h"
#include "vtkResliceImageViewerMeasurements.h"
#include <QMainWindow>

// Forward Qt class declarations
class Ui_QtVTKRenderWindows;

class QtVTKRenderWindows : public QMainWindow
{
  Q_OBJECT
public:

  // Constructor/Destructor
  QtVTKRenderWindows(int argc, char *argv[]);
  ~QtVTKRenderWindows() {}

public slots:

  virtual void slotExit();

protected:
  vtkSmartPointer< vtkResliceImageViewer > riw[3];
  //vtkSmartPointer< vtkImagePlaneWidget > planeWidget[3];
  //vtkSmartPointer< vtkDistanceWidget > DistanceWidget[3];
  //vtkSmartPointer< vtkResliceImageViewerMeasurements > ResliceMeasurements;

protected slots:

private:

  // Designer form
  Ui_QtVTKRenderWindows *ui;
};

#endif // QtVTKRenderWindows_H
