Install Mingw/Windows HOWTO
===========================

Building duke under Windows is more complicated than building on Linux but it definitely is possible.  
Here is a step by step tutorial to explain exactly that, so fasten your seatbelts, here we go.

MinGW - The right version
-------------------------

For the most part of the compilation process you'll only need [MinGW](http://www.mingw.org/), but for a few libs you'll also need [MSYS](http://www.mingw.org/wiki/MSYS).  

Installing MSYS will happily install MinGW too so we'll opt for that option.  
By reading [MSYS's web page](http://www.mingw.org/wiki/MSYS), you'll learn there's two way of installing MSYS the old way and the recent one.  
Go for the [recent one](http://www.mingw.org/wiki/Getting_Started) and as recommended download the utility called [mingw-get-inst](http://sourceforge.net/projects/mingw/files/Installer/mingw-get-inst/)

* Launch mingw-get-inst 
* select the ```use prepackaged repository catalog```
* accept the licence
* install in C:\MinGW
* in the MinGW compiler suite select
 * C compiler
 * C++ compiler
 * MSYS Basic System
 * MinGW Developer Toolkit

Once installed you'll have an MinGW Shell available in your Start Menu.

Qt - from Sources
-----------------

Qt binaries for Windows are only available for an old version of MinGW so we'll have to build our own version.  

* Download qt-everywhere from the [Download page](http://qt-project.org/downloads)
 * the link is not easy to find : search for ```source``` in the page and download the [zip file](http://releases.qt-project.org/qt4/source/qt-everywhere-opensource-src-4.8.2.zip)
* unzip in ```C:\lib\qt-everywhere-opensource-src-4.8.2\```
* open a shell ( cmd.exe ) in ```C:\lib\qt-everywhere-opensource-src-4.8.2\```
 * warning : do not use MinGW Shell here, Qt's configure script does not work within this shell, use cmd.exe
* ```set PATH=c:\MinGW\bin;%PATH%;c:\MinGW\msys\1.0\bin```
 * Qt's configure script needs access to MinGW's bin folder and to a perl interpreter ( hopefully there's one shipped with MSYS ) so let's configure the PATH
* ```configure -release -opensource -developer-build -platform win32-g++-4.6 -no-openssl -no-s60 -nomake demos -nomake examples -arch windows```
 * to configure Qt to your needs have a look at the [configure switchs](http://doc-snapshot.qt-project.org/4.8/configure-options.html)
* ```mingw32-make```
 * warning : do not use ```make```, it would point to MSYS's make and compilation would abort with strange error messages, stick to MinGW's ```mingw32-make```
 * use ```mingw32-make -j4``` if you have 4 cores to speedup the compilation process
 * take a coffee
 * take another coffee

Protobuf
--------

Now you need to compile [Google's protocol buffer](http://code.google.com/p/protobuf/).  

* Download the [sources](http://code.google.com/p/protobuf/downloads/list)
* unzip in ```C:\lib\protobuf-2.4.1\```
* launch MinGW Shell from Start menu
* ```cd /C/lib/protobuf-2.4.1/```
 * please note that in MinGW Shell path are Unix style.
* ```./configure --prefix /C/lib/protobuf```
 * we want the distribution folder to be C:\lib\protobuf
* ```make```
 * or ```make -jX``` if you have X cores...
* ```make install```

This will install everything into ```C:\lib\protobuf```.  
You can now remove the sources if you want (```C:\lib\protobuf-2.4.1```).

Building GLEW
-------------

* Download GLEW sources from [the website](http://glew.sourceforge.net/)
 * Choosing the git version didn't work for me, I had to use the tarball.
* unzip in ```C:\lib\glew-1.8.0\```
* open an MinGW Shell
* ```cd /C/lib/glew-1.8.0```
* type the following commands
> gcc -DGLEW_NO_GLU -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o src/glew.o -c src/glew.c  
> gcc -shared -Wl,-soname,libglew32.dll -Wl,--out-implib,lib/libglew32.dll.a    -o lib/glew32.dll src/glew.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32  
> ar cr lib/libglew32.a src/glew.o  
> gcc -DGLEW_NO_GLU -DGLEW_MX -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o src/glew.mx.o -c src/glew.c  
> gcc -shared -Wl,-soname,libglew32mx.dll -Wl,--out-implib,lib/libglew32mx.dll.a -o lib/glew32mx.dll src/glew.mx.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32  
> ar cr lib/libglew32mx.a src/glew.mx.o

You should now have glew32.dll, libglew32.a and libglew32.dll.a in ```C:\lib\glew-1.8.0\lib\```

Building Optional Libraries
===========================

Building lib jpeg turbo
-----------------------

* Download [nasm](http://www.nasm.us/) from the download page
* unzip the archive and put nasm.exe into C:\MinGW\bin
* Download [libturbojpeg](http://libjpeg-turbo.virtualgl.org/) sources from the [website](http://sourceforge.net/projects/libjpeg-turbo/files/latest/download?source=files)
* unzip in ```C:\lib\libjpegturbo-1.2.1\```
* open an MinGW Shell
* ```./configure --prefix /C/lib/libjpeg-turbo```
* ```make```
 * or ```make -jX``` if you have X cores...
* ```make install```

This will install libjpeg turbo into ```C:\lib\libjpeg-turbo```
You can now remove the sources if you want (```C:\lib\libjpegturbo-1.2.1\```).