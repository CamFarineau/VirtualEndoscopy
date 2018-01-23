# VirtualEndoscopy<br />


################################<br />
Projet Majeure - 5IMI @CPELyon<br />
Farineau Camille - Ranc Nicolas<br />
################################<br />
<br />
Please read the PDF File "VirtualEndoscopy" for more details about the project<br />


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
     /path/to/VTK</p><br/>


# Reference

<p>Please read UsefulWebsite.txt attachment.</p>




