#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include <vtkRenderWindow.h>


#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkJPEGReader.h>
#include <vtkImageMapper3D.h>
#include <vtkImageViewer2.h>


//#include "mainwindow.h"
#include <QApplication>
#include <QLCDNumber>

#include <QPushButton>
#include <QGridLayout>
#include <QSlider>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget fenetre;

    QLCDNumber *number1 = new QLCDNumber();
    QLCDNumber *number2 = new QLCDNumber();
    QLCDNumber *number3 = new QLCDNumber();

    QSlider *slider1 = new QSlider(Qt::Horizontal);
    QSlider *slider2 = new QSlider(Qt::Horizontal);
    QSlider *slider3 = new QSlider(Qt::Horizontal);

    QPushButton *bouton1 = new QPushButton("Fenetre 1");
    QPushButton *bouton2 = new QPushButton("Fenetre 2");
    QPushButton *bouton3 = new QPushButton("Fenetre 3");


    QVBoxLayout *layout1 = new QVBoxLayout;
    layout1->addWidget(number1);
    layout1->addWidget(slider1);
    layout1->addWidget(bouton1);

    QVBoxLayout *layout2 = new QVBoxLayout;
    layout2->addWidget(number2);
    layout2->addWidget(slider2);
    layout2->addWidget(bouton2);

    QVBoxLayout *layout3 = new QVBoxLayout;
    layout3->addWidget(number3);
    layout3->addWidget(slider3);
    layout3->addWidget(bouton3);

    QGridLayout *layout_global = new QGridLayout;

    layout_global->addLayout(layout1,0,0);
    layout_global->addLayout(layout2,0,1);
    layout_global->addLayout(layout3,1,0);

    slider1->setRange(0,150);
    slider2->setRange(0,150);
    slider3->setRange(0,150);

    QObject::connect(slider1, SIGNAL(valueChanged(int)), number1, SLOT(display(int))) ;
    QObject::connect(slider2, SIGNAL(valueChanged(int)), number2, SLOT(display(int))) ;
    QObject::connect(slider3, SIGNAL(valueChanged(int)), number2, SLOT(display(int))) ;

    // Read the data
    vtkImageData *input=0;

    std::string dirname;
    dirname="/home/nico/Projet_Majeure/Head";
    vtkDICOMImageReader *dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(dirname.c_str());
    dicomReader->Update();
    input=dicomReader->GetOutput();

    int dim[3];
    input->GetDimensions(dim);
    std::cout<<"Dimensions :"<<dim[0]<<" , "<<dim[1]<<" , "<<dim[2]<<";"<<std::endl;

    dicomReader;

    // Display the result
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();

    vtkSmartPointer<vtkImageViewer2> imageViewer =
    vtkSmartPointer<vtkImageViewer2>::New();
    imageViewer->SetInputData(input);
    imageViewer->SetSize(640, 512);
    imageViewer->SetupInteractor(renderWindowInteractor);
    imageViewer->GetRenderer()->ResetCamera();
    imageViewer->GetRenderer()->SetBackground(1,0,0); //red

    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();



    fenetre.setLayout(layout_global);

    fenetre.show();

    return app.exec();

}
