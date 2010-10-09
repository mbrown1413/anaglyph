
==================
Anaglyph Generator
==================

This is an anaglyph generator that works with multiple different anaglyph
methods.  Each method implements a simple interface.  Each method has its own
executable compiled for it.

-----------
Source Tree
-----------
A summary of where everything is:

bin/
    The executables for each method and object files for c files directly
    inside "/".

images/
    Some left and right eye anaglyph images you can use as examples.

methods/
    Contains interface.h and one C file for each anaglyph method.

------------
Dependencies
------------
Required:
  * gcc
  * OpenCV
  * make
  * levmar

Optional:
  * OpenCV FFMpeg Support - If you want to read video.
  * OpenMP - Used for multithreading.  To turn this off, see the multithreading
             option in settings.mk

Mac Specifics
`````````````
MacPorts can be used to install all of the above except for levmar.  Installing
OpenCV will also install FFMpeg support and OpenMP.  For more details on Mac
installation, read OpenCV's documentation:

http://opencv.willowgarage.com/wiki/Mac_OS_X_OpenCV_Port

---------
Compiling
---------
First, copy "settings.mk.default" to "settings.mk".  Next, open "settings.mk"
and read through the options.  You will need to change some of these before
compiling.

To compile, type the "make" command inside the root of the repository.  The
executables generated, one for each method, will be placed in the bin/
directory.

-------
Running
-------
Run one of the executables in bin/<method_name> and pass in a left eye image
and a right eye image as arguments.  There are some sample anaglyph images in
the images/ directory, with the files clearly labeled as corresponding left and
right eye images.

-----------------
Creating a Method
-----------------
If you want to create your own anaglyph method, create a file named
"methods/<method_name>.c".  This file will automatically be compiled after
following the steps in the "Compiling" section and the executable
file will be put in bin/.

Look in methods/interface.h.  This is the exact set of function prototypes that
you need to have in your .c file.  Read methods/interface.h for documentation
on exactly what these functions do, and when they are called.
