/*
**    CPE Lyon
**    2018 Camille FARINEAU / Nicolas Ranc
**    Projet Majeur - Virtual Endoscopy
**
**    DICOMIMageReaderFileCollection.cxx
**    Main application :
**          Read DICOM data
**          Create GUI : 4 viewers - 3 vtkResliceImageViewer + 1 vtkGenericOpenGLRenderWindow
**          Each vtkResliceImageViewer window show one slice corresponding to one slide of the volume according to an axis (X,Y,Z)
**          This volume is the contraction of all slides in accordance with all axis.
**          vtkGenericOpenGLRenderWindow window represents the surface of the volume created by Marching Cubes Algorithm.
**
*/

//VTK headers

#include "vtkSmartPointer.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include "vtkAxesActor.h"
#include <vtkCommand.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageActor.h>
#include <vtkProperty.h>
#include <vtkPropPicker.h>
#include <vtkRenderer.h>
#include <vtkMath.h>
#include <vtkUnsignedCharArray.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkFileOutputWindow.h>
#include "vtkResliceImageViewer.h"
#include <vtkStripper.h>
#include "vtkCameraActor.h"
#include "vtkDecimatePro.h"
#include "vtkSphereSource.h"
#include <vtkActor.h>
#include <vtkTextProperty.h>

#include <vtkGenericOpenGLRenderWindow.h>

//! Test: Add GPU Ray Cast - doesn't work
//!#include "vtkGPUVolumeRayCastMapper.h"

//QT headers

#include <QApplication>
#include <QVTKOpenGLWidget.h>
#include <QSurfaceFormat>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include "QVTKOpenGLWidget.h"

//VirtualEndoscopy headers

#include "KeyPressInteractorStyle.h"
#include "KeyPressInteractorNavigationStyle.h"
#include "vtkImageInteractionCallback.h"

using namespace std;

//VTK macro - Allow New() usage
vtkStandardNewMacro(KeyPressInteractorStyle);
vtkStandardNewMacro(KeyPressInteractorNavigationStyle);


//Compute number of triangle inside a vtkPolyData
auto NumberofTriangles = [](vtkPolyData* pd) {
    vtkCellArray* cells = pd->GetPolys();
    vtkIdType npts = 0;
    vtkIdType* pts;
    auto numOfTriangles = 0;
    for (auto i = 0; i < pd->GetNumberOfPolys(); ++i)
    {
        int c = cells->GetNextCell(npts, pts);
        if (c == 0)
        {
            break;
        }
        // If a cell has three points it is a triangle.
        if (npts == 3)
        {
            numOfTriangles++;
        }
    }
    return numOfTriangles;
};


//Main function
int main(int argc, char *argv[])
{
    //Dataset needs to be set by command line ./executable /path/to/dataset
    //usually /path/to/dataset = ../data/Woman_Head or ../data/Pelvis
    if ( argc <= 1 )
    {
        cout << "Usage: " << argv[0] << " <dicom folder>" << endl;
        return 1;
    }

    /**************************************************************************/
    //Extract from https://www.vtk.org/Wiki/VTK/Examples/Cxx/Utilities/FileOutputWindow
    //Usage: stock error output into "output_errors.txt" file
    vtkSmartPointer<vtkFileOutputWindow> fileOutputWindow =  vtkSmartPointer<vtkFileOutputWindow>::New();
    fileOutputWindow->SetFileName( "output_errors.txt" );

    // Note that the SetInstance function is a static member of vtkOutputWindow.
    vtkOutputWindow::SetInstance(fileOutputWindow);
    /***************************************************************************/


    //Read dicom files
    std::string directoryName = argv[1];
    vtkSmartPointer<vtkDICOMImageReader> DICOMReader = vtkSmartPointer<vtkDICOMImageReader>::New();
    // Read the input files
    DICOMReader->SetDirectoryName(directoryName.c_str());
    cout << "Directory name: " << DICOMReader->GetDirectoryName() << endl;
    DICOMReader->Update();

    //Extent returns number of slices on x , y and z axis
    //std::cout<<"Extent: "<<DICOMReader->GetDataExtent()[0]<<" , "<<DICOMReader->GetDataExtent()[1]<<" , "<<DICOMReader->GetDataExtent()[2]<<" , "<<DICOMReader->GetDataExtent()[3]<<" , "<<DICOMReader->GetDataExtent()[4]<<" , "<<DICOMReader->GetDataExtent()[5]<<std::endl;

    /***************************************************************************/
    //Main application creation
    //QtVTKRenderWindows represents the background window, main_widget reprensents 4 viewers inside
    // .ui file is not use, it wasn't working with it.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
    QApplication app(argc, argv);
    QMainWindow *QtVTKRenderWindows = new QMainWindow();
    //QtVTKRenderWindows->resize(1240, 850);
    QtVTKRenderWindows->resize(1050, 700);
    QWidget *main_widget = new QWidget();
    QWidget *gridLayoutWidget = new QWidget(main_widget);
    gridLayoutWidget->setGeometry(QRect(10, 30, 1000, 650));
    QGridLayout *gridLayout_2 = new QGridLayout(gridLayoutWidget);
    gridLayout_2->setContentsMargins(0, 0, 0, 0);
    QVTKOpenGLWidget *view2 = new QVTKOpenGLWidget(gridLayoutWidget);
    gridLayout_2->addWidget(view2, 1, 0, 1, 1);
    QVTKOpenGLWidget *view4 = new QVTKOpenGLWidget(gridLayoutWidget);
    view4->setObjectName(QStringLiteral("view4"));
    gridLayout_2->addWidget(view4, 0, 1, 1, 1);
    QVTKOpenGLWidget *view3 = new QVTKOpenGLWidget(gridLayoutWidget);
    gridLayout_2->addWidget(view3, 1, 1, 1, 1);
    QVTKOpenGLWidget* view1 = new QVTKOpenGLWidget(gridLayoutWidget);
    gridLayout_2->addWidget(view1, 0, 0, 1, 1);
    QtVTKRenderWindows->setCentralWidget(main_widget);

    /***************************************************************************/

    //Create 3 2D viewers which show slices according to x,y or z axis
    vtkSmartPointer<vtkResliceImageViewer> imageViewer[3];

    // Associate each imageviewer with a renderWindow
    for(int i=0; i<3; i++)
    {
        imageViewer[i] = vtkSmartPointer<vtkResliceImageViewer>::New();
        imageViewer[i]->SetInputData(DICOMReader->GetOutput());
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        imageViewer[i]->SetRenderWindow(renderWindow);
    }


    //Init interactior
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    //Associate each viewer with the coordinate renderwindow and interactor
    view1->SetRenderWindow(imageViewer[0]->GetRenderWindow());
    imageViewer[0]->SetupInteractor(view1->GetInteractor());
    view2->SetRenderWindow(imageViewer[1]->GetRenderWindow());
    imageViewer[1]->SetupInteractor(view2->GetInteractor());
    view3->SetRenderWindow(imageViewer[2]->GetRenderWindow());
    imageViewer[2]->SetupInteractor(view3->GetInteractor());


    //Init links between 2D slices and mouse
    vtkSmartPointer<vtkCamera> camera[2];
    vtkSmartPointer<vtkCornerAnnotation> cornerAnnotation[3];
    vtkSmartPointer<vtkImageInteractionCallback> callback[3];
    vtkSmartPointer<vtkPropPicker> propPicker[3];
    vtkImageActor* imageActor[3];
    vtkSmartPointer<KeyPressInteractorStyle> style[3];

    for (int i = 0; i < 3; i++)
    {
        // Picker
        // Get pixel coordinates
        // Picker to pick pixels
        propPicker[i] = vtkSmartPointer<vtkPropPicker>::New();
        propPicker[i]->PickFromListOn();

        // Give the picker a prop to pick
        imageActor[i] = imageViewer[i]->GetImageActor();
        propPicker[i]->AddPickList(imageActor[i]);

        // disable interpolation, so we can see each pixel
        imageActor[i]->InterpolateOff();


        // Annotate the image with window/level and mouse over pixel
        // information
        cornerAnnotation[i] = vtkSmartPointer<vtkCornerAnnotation>::New();
        cornerAnnotation[i]->SetLinearFontScaleFactor(2);
        cornerAnnotation[i]->SetNonlinearFontScaleFactor(1);
        cornerAnnotation[i]->SetMaximumFontSize(20);
        cornerAnnotation[i]->SetText(0, "Off Image");
        cornerAnnotation[i]->GetTextProperty()->SetColor(1, 0, 0);

        imageViewer[i]->GetRenderer()->AddViewProp(cornerAnnotation[i]);

        // Callback listens to MouseMoveEvents invoked by the interactor's style
        callback[i] = vtkSmartPointer<vtkImageInteractionCallback>::New();
        callback[i]->SetViewer(imageViewer[i]);
        callback[i]->SetAnnotation(cornerAnnotation[i]);
        callback[i]->SetPicker(propPicker[i]);

        style[i] = vtkSmartPointer<KeyPressInteractorStyle>::New();
        style[i]->SetViewer(imageViewer[i]);
        style[i]->SetPicker(propPicker[i]);

        if(i==0)
            view1->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );
        else if(i==1)
            view2->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );
        else if(i==2)
            view3->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style[i] );

        style[i]->AddObserver(vtkCommand::MouseMoveEvent, callback[i]);

        imageViewer[i]->SetSliceOrientation(i);
        imageViewer[i]->SetColorLevel(-100);
        imageViewer[i]->SetColorWindow(4000);
        imageViewer[i]->Render();
        imageViewer[i]->GetRenderer()->ResetCamera();

        //2 slices are returned, need a 180 degrees turn
        if(i<2){
            camera[i] =  imageViewer[i]->GetRenderer()->GetActiveCamera();
            double rotation = 180.0;
            camera[i]->Roll(rotation);
        }
    }

    //Init interactor
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();


    //Marching Cubes. Render a specific surface corresponding to the specific isovalue
    double isoValue=-90;

    //Input : volume
    vtkSmartPointer<vtkImageData> volume = vtkSmartPointer<vtkImageData>::New();
    volume->DeepCopy(DICOMReader->GetOutput());

    //Algorithm extract specific isovalue surface from this volume
    vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
    surface->SetInputData(volume);
    surface->ComputeNormalsOn();
    surface->ComputeGradientsOn();
    surface->ComputeScalarsOn();
    surface->SetValue(0,isoValue);

    //POssible to set a specific number of surface from a range
    /* double range[2];
    range[0]=-90.0f;
    range[1]=-120.0f;
    surface->GenerateValues(4,range);*/

    surface->Update();


    /***************************************************************************/
    //Some ideas come from https://www.vtk.org/Wiki/VTK/Examples/Cxx/Meshes/Decimation

    std::cout << "Before Decimation." << std::endl;
    std::cout << "There are: " << NumberofTriangles(surface->GetOutput()) << " triangles." << std::endl;

    vtkSmartPointer<vtkDecimatePro> deci = vtkSmartPointer<vtkDecimatePro>::New();
    deci->SetInputConnection(surface->GetOutputPort());
    deci->SetTargetReduction(0.9); //is 0.9 means 90% reduction -> if 100 triangles, only 10
    deci->SetAbsoluteError(0.01);
    deci->SetFeatureAngle(30);
    deci->SetErrorIsAbsolute(1);
    deci->AccumulateErrorOn();
    deci->PreserveTopologyOff();
    deci->SplittingOn();
    deci->BoundaryVertexDeletionOn();
    deci->Update();
    std::cout << "After Decimation." << std::endl;
    std::cout << "There are: " << NumberofTriangles(deci->GetOutput())
              << " triangles." << std::endl;

    /***************************************************************************/

    //Set background color
    vtkSmartPointer<vtkRenderer> surfaceRenderer = vtkSmartPointer<vtkRenderer>::New();
    surfaceRenderer->SetBackground(.1, .2, .3);

    //Add renderer in the window
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> surfaceRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    surfaceRenderWindow->AddRenderer(surfaceRenderer);

    //Set window in viewer
    view4->SetRenderWindow(surfaceRenderWindow);

    //Set interactor in the render window
    vtkSmartPointer<vtkRenderWindowInteractor> surfaceInteractor = view4->GetInteractor();
    surfaceInteractor->SetRenderWindow(surfaceRenderWindow);

    //Set mapper - Marching cubes output goes inside mapper
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(surface->GetOutput());
    mapper->ScalarVisibilityOff();

    //Catch center of 3D model - Set camera focal point on it
    double middlePoint[3], bounds[6];
    mapper->GetBounds(bounds);
    middlePoint[0] = (bounds[1] + bounds[0])/2;
    middlePoint[1] = (bounds[3] + bounds[2])/2;
    middlePoint[2] = (bounds[5] + bounds[4])/2;

    //Init camera
    vtkSmartPointer<vtkCamera> aCamera =   vtkSmartPointer<vtkCamera>::New();
    aCamera =  surfaceRenderer->GetActiveCamera();

    //Set initial position and focal point (where camera watches)
    aCamera->SetPosition(450, 1000, 0);
    aCamera->SetFocalPoint(middlePoint[0], middlePoint[1],middlePoint[2]);
    aCamera->SetViewUp(0,0,-1); //Angle correctly the camera

    //Set actor (3D model) - Set diffuse/specular color
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(1, .49, .25);
    actor->GetProperty()->SetSpecular(.45);
    actor->GetProperty()->SetSpecularPower(20);
    actor->GetProperty()->SetDiffuse(.9);
    //actor->GetProperty()->BackfaceCullingOn();

    /***************************************************************************/
    //Extract comes from https://www.vtk.org/Wiki/VTK/Examples/Cxx/Picking/HighlightPickedActor
    //Associate camera with a small sphere - if collision beween model and sphere, camera doesn't move
    vtkSmartPointer<vtkSphereSource> source = vtkSmartPointer<vtkSphereSource>::New();

    double radius = 0.7;
    source->SetRadius(radius);
    source->SetCenter(aCamera->GetPosition());
    source->SetPhiResolution(11);
    source->SetThetaResolution(21);
    vtkSmartPointer<vtkPolyDataMapper> mapper_camera =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper_camera->SetInputConnection ( source->GetOutputPort());
    vtkSmartPointer<vtkActor> actor_camera =
            vtkSmartPointer<vtkActor>::New();
    actor_camera->SetMapper(mapper_camera);
    double r, g, b;
    r = vtkMath::Random(.4, 1.0);
    g = vtkMath::Random(.4, 1.0);
    b = vtkMath::Random(.4, 1.0);
    actor_camera->GetProperty()->SetDiffuseColor(r, g, b);
    actor_camera->GetProperty()->SetDiffuse(.2);
    actor_camera->GetProperty()->SetSpecular(.2);
    actor_camera->GetProperty()->SetSpecularColor(1.0,1.0,1.0);
    actor_camera->GetProperty()->SetSpecularPower(30.0);
    actor_camera->GetProperty()->SetOpacity(0.0);
    surfaceRenderer->AddActor ( actor_camera );

    /***************************************************************************/

    //Choose between real surface and decimated surface
    deci->Update();
    //surface->Update();

    //Add volume to the render
    surfaceRenderer->AddActor(actor);
    //Reset camera when we launch the executable
    surfaceRenderer->ResetCameraClippingRange();

    //Render scene
    surfaceRenderWindow->Render();

    //Specific style navigation for the camera interactor. Allow keyboard interactions
    vtkSmartPointer<KeyPressInteractorNavigationStyle> styleNav =vtkSmartPointer<KeyPressInteractorNavigationStyle>::New();
    styleNav->SetCamera(aCamera);
    styleNav->SetInteractor(view4->GetInteractor());

    //Choose between real surface and decimated surface
    styleNav->SetSurface(deci->GetOutput());
    styleNav->SetSurfaceCollision(deci);
    //styleNav->SetSurface(surface->GetOutput());

    //Sphere and camera interactor are associated, when camera goes forward sphere goes forward
    styleNav->SetSphere(source);
    styleNav->SetIntersectionPolyDataFilter();
    surfaceInteractor->SetInteractorStyle( styleNav );
    surfaceInteractor->Start();

    //Each 2D viewer has the 3D viewer setting
    for(int j=0;j<3;j++){
        style[j]->SetCamera(aCamera);
        style[j]->SetInteractor(surfaceInteractor);
        style[j]->SetViewerNav(surfaceInteractor->GetRenderWindow());
    }

    /***************************************************************************/
    //Extract from https://www.vtk.org/Wiki/VTK/Examples/Cxx/Visualization/DisplayCoordinateAxes
    //Show 3D benchmark at the bottom left corner
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    vtkSmartPointer<vtkOrientationMarkerWidget> widget =
            vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    widget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
    widget->SetOrientationMarker( axes );
    widget->SetInteractor( surfaceInteractor );
    widget->SetViewport( 0.0, 0.0, 0.2, 0.2 );
    widget->SetEnabled( 1 );
    widget->InteractiveOff();
    /***************************************************************************/


    //Show QT windows
    QtVTKRenderWindows->show();

    app.exec();
    return 0;
}
