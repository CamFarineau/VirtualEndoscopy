# VirtualEndoscopy<br />


################################<br />
Projet Majeure - 5IMI @CPELyon<br />
Farineau Camille - Ranc Nicolas<br />
################################<br />
<br />
[a relative link](doc/html/index.html)

Please read the File "/misc/VirtualEndoscopy.pdf" for more details about the project<br />

<p>Link to the repository of the project: <i>https://github.com/CamFarineau/VirtualEndoscopy</i></p>

<p>This project is part of our study in CPE Lyon in Image-Modelisation-Computing. The purpose of the project is to create a simple viewer for virtual endoscopy. The goal is to visualize the slices from DICOM files obtained with IRM or CT scans. And then it will reconstruct, with Marching Cubes, a particular isosurface in order to navigate into the 3D model of the patient and detect symptom of tumors etc. You can navigate into the 3D model with the keyboard or a controller.</p>

<p>Don't hesitate to look at the documentation inside the doc folder or to look at the documentation of VTK if you have any question.</p>

![Alt text](./misc/Overview.png?raw=true "Overview of the app")

# Requirements
<p>This project was done and tested under Linux 16.04 environment.</p>
<p>VTK, QT and OpenGL need to be install</p>


## QT 5.7.0

### Version

<p>Lastest QT version need to be install.<p>
<p>In deed, VTK 8.1.0 need ressources and algorithms which run only with QT 5.</p>
<p><i>QT4 is not usable for this project.</i></p>


### Install

<p>https://download.qt.io/official_releases/qt/5.7/5.7.0/</p>
<p>Download : <i>qt-opensource-linux-x64-5.7.0.run</i></p>
<p>chmod +x qt-opensource-linux-x64-5.2.1.run</p>
<p>./qt-opensource-linux-x64-5.2.1.run</p>

## VTK 8.1.0

### Download

<p> https://www.vtk.org/download/ </p>
<p> Download : <i>VTK-8.1.0.zip</i></p>

### Install

<p>VTK needs to be build with QT. </p><br/>

<p>According to <i>https://www.vtk.org/Wiki/VTK/Configure_and_Build</i> :</p>
<p>$ cd path/to/vtk </p>
<p>$ mkdir build </p>
<p>$ cd build </p>
<p>$ cmake -DVTK_QT_VERSION:STRING=5 \  <br/>
     -DQT_QMAKE_EXECUTABLE:PATH=/path/to/qt5.7.0-install/5.7.0/gcc_64/bin/qmake \ <br/>
     -DVTK_Group_Qt:BOOL=ON \ <br/>
     -DCMAKE_PREFIX_PATH:PATH=/path/to/qt.5.7.0-install/5.7.0/gcc_64/lib/cmake  \ <br/>
     -DBUILD_SHARED_LIBS:BOOL=ON <br/>
     /path/to/VTK</p>

## Use DualShock 4 controller on Ubuntu

<p>Please follow this link to install the DualShock 4 Controller for Ubuntu and map the controller to the keyboard: <i>https://github.com/chrippa/ds4drv</i></p>
<p> An example of the config file (ds4drv.conf) we used for controller mapping is located inside the misc folder.</p>

# Compile and run program

## CMAKE

<p><b>Important: Open the CMakeLists.txt located as the root of the VirtualEndoscopy folder. Change the 3 "set" command lines with you own location: </b></p>
<p>$ set(CMAKE_PREFIX_PATH /Path/To/VTK)</p>
<p>$ set(QT_QMAKE_EXECUTABLE /Path/To/qmake) (usually : /usr/lib/x86_64-linux-gnu/qt5/bin/qmake)</p>
<p>$ set(VTK_DIR /Path/To/VTK/Build/)</p>

## Compile

<p>$ cd /location/of/VirtualEndoscopy/folder</p>
<p>$ mkdir build</p>
<p>$ cd build</p>
<p>$ cmake ..</p>
<p>$ make</p>

## Run
<p>$ ./VirtualEndoscopy /location/of/the/Folder/containing/DICOM/Files</p>

<p><b>!! You need to pass the location of the folder where all your DICOM files of interest are located as the first argument when you execute the program !!</b></p>

<p>You can download some test files from the Visible Human Project here: <i>https://mri.radiology.uiowa.edu/visible_human_datasets.html</i></p>

# Controls

## Mouse

### On Slice Viewer
<p> Left click and move : Change window and level </p>
<p> Right click and move : Zoom In and Out </p>
<p> Scroll wheel : Change the slice number </p>

### On 3D Viewer (not recommended, use keyboard instead)
<p> Left click and move : Controls the camera </p>
<p> Right click and move : Zoom In and Out </p>

## Keyboard

### On Slice Viewer
<p> Place the cursor of the mouse at a desired location on a slice and press "P" : Will set the location of the camera of the 3D Viewer to the current mouse location on the slice </p>

### On 3D Viewer
<p> "Z" : Camera moves forward </p>
<p> "S" : Camera moves backward </p>
<p> "Arrow Keys" : Controls the orientation of the camera </p>

## Controller
<p> Map your favorite controller (for example the DualShock 4 controller from Sony) to the keyboard: Arrows keys on one of the joypad, left mouse button to cross button on the controller etc. and navigate into the 3D viewer naturally with the controller.</p>
<p>Look at the config file inside the misc folder (ds4.conf) for more information how to map correctly the controller.</p>

# Documentation

<p>To look at the documentation open the "index.html" file inside the /doc/html folder.</p>

# References

## Dataset

<p>All the datasets we used come from the Visible Human Project and can be found here: <i>https://mri.radiology.uiowa.edu/visible_human_datasets.html</i></p>

## 3D Slicer

<p> We looked and inspired from the 3D Slicer implementation of the endoscopy module: <i>https://www.slicer.org/wiki/Documentation/4.8/Modules/Endoscopy</i></p>

## VTK

<p> Some parts of the code come from the VTK documentation or VTK examples (details of the part used in the src files).</p>
<p>VTK: <i>https://www.vtk.org/</i></p>
<p>VTK Examples in C++: <i>https://lorensen.github.io/VTKExamples/site/Cxx/</i></p>
<p>VTK Marching Cubes: <i>https://www.vtk.org/Wiki/VTK/Examples/Cxx/Modelling/MarchingCubes</i></p>
<p>VTK Four Pane Viewer: <i>https://github.com/Kitware/VTK/tree/master/Examples/GUI/Qt/FourPaneViewer</i></p>

## vtkSMPTools (Multithreading) & VTK-m (GPU)
<p><i>https://blog.kitware.com/simple-parallel-computing-with-vtksmptools-2</i></p>
<p><i>http://m.vtk.org/Wiki/images/3/3b/VTK_SMP_Guide.pdf</i></p>

## Driver for Dualshock 4 controller
<p><i>https://github.com/chrippa/ds4drv</i></p>
