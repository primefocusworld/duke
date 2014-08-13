Duke
====

Duke is an **opensource high resolution image and sequence viewer** dedicated to visual effect and post production companies.

Binaries
--------

### Stable v2.0

* [Linux 64 / g++ 4.8](http://www.bbteam.fr:8090/job/duke-master/compiler=gcc/lastSuccessfulBuild/artifact/dist/*zip*/dist.zip)
* [Linux 64 / clang 3.3](http://www.bbteam.fr:8090/job/duke-master/compiler=clang/lastSuccessfulBuild/artifact/dist/*zip*/dist.zip)
* no Windows nor MacOsX binaries for now...


### Develop

* [Linux 64 / g++ 4.8](http://www.bbteam.fr:8090/job/duke-develop/compiler=gcc/lastSuccessfulBuild/artifact/dist/*zip*/dist.zip)
* [Linux 64 / clang 3.3](http://www.bbteam.fr:8090/job/duke-develop/compiler=clang/lastSuccessfulBuild/artifact/dist/*zip*/dist.zip)

Current features
----------------

* image viewer / sequence player
* look ahead cache with multithreaded load/decode
* `space` : play pause
* `left` `right` : go to previous/next frame (hold `Ctrl` to jump 25 frames)
* `+` `-` `*` : increase/decrease/reset exposure
* `o` : show/hide metadata
* `f` : cycle through fit mode ( actual, fit inner, fit outer )
* `s` : display/hide cache state, framerate
* `home` `end` : go to begin, end of playlist
* `r` `g` `b` `a` : toggle R/G/B/A filter channel
* mouse drag and mouse wheel to pan/zoom
* timeline scrub
* frame number from file name

* OpenColorIO integration: load and handle Lut files and Colorspace changes.
Colorspaces available from nuke configuration: linear, sRGB, sRGBf, Cineon, Gamma1.8, Gamma2.2, Panalog, REDLog, ViperLog, AlexaV3logC, PLogLin, SLog and raw.

Use the --outputspace and --viewinglut commands to use OCIO.
example for Colorspace changes: ./src/duke/duke ~/Desktop/video..exr --outputspace Cineon
example for Luts files: ./src/duke/duke ~/Desktop/video..exr --viewinglut ~/pathtoluthile/luts/cineon.spi1d
The default Colorspace is linear.

We aim at
---------

* support for Look Up Tables
* split view to compare tracks
* basic color grading
* movie playback
* recording to file sequences or movies
* sound support
* 3D support
* Qt interface


Compilation
-----------

* Getting the source

> git clone --recursive https://github.com/primefocusworld/duke.git
cd duke

Current branch should be "develop", but to be sure
git checkout develop


* Compilation


1. Load the dev environment with 
module load dev/duke

2. Export the OCIO configuration file with:
export OCIO = "~/duke/dependencies/nuke-default/config.ocio" or wherever you have put the config.ocio file.

3. In the duke folder, create a build directory with
mkdir build
cd build

if ccache is not installed on your machine then use the following ccmake command line:
ccmake -D CMAKE_CXX_COMPILER=$(which g++)

4. Create the makefiles
ccmake ..
ccmake opens a configuration window. the key c is use to configure, key g to generate once configuration is done. You can change all the variables, but they should already be pointing to the correct libs. key g generates and quit 

5. Makefile is created, so build the project with make
make

The executable should be duke/build/src/duke/duke, so provided your current folder is duke/build/, just run
./src/duke/duke

    see INSTALL file for more information
    
You might need the following libraries to make it run on your machine:
libxtst-dev
libxrandr-dev
libxext-dev 
libxi-dev
libglu-dev
glibc-devel 
glibc-static
libXrandr-devel.x86_64
libmesa-dev
    
**Known issues**

GCC 4.7 is not supported due to an issue with `std::chrono`, you will need GCC 4.8.
If you use Clang and don't use [Clang's standard library](http://libcxx.llvm.org/) then you will also need to rely on GCC 4.8 standard library.


More informations 
-----------------

* Duke public repository
http://github.com/mikrosimage/duke

* The github issues module for bug report
https://github.com/mikrosimage/duke/issues


License
-------

    see COPYING file


Credits
-------

Duke has been initiated by HD3D² R&D project, sustained by Cap Digital, the French business cluster for digital content and services.

The project is maintained as an open source project by [Mikros Image](http://www.mikrosimage.eu).

Duke uses
* [GLFW](https://github.com/elmindreda/glfw)
* [GLM](https://github.com/Groovounet/glm)
* [concurrent_utils](https://github.com/mikrosimage/concurrent_utils)
* [light_sequence_parser](https://github.com/gchatelet/light_sequence_parser)
