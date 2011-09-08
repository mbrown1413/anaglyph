
==================
Anaglyph Generator
==================

This is an anaglyph generator that can use different anaglyph generation
methods.  Each method implements a simple interface.  Each method has its own
executable compiled for it.

-----------
Source Tree
-----------

bin/
    The executables for each method and object files for c files directly
    inside "/".

images/
    Some left and right eye anaglyph images you can use as examples.

methods/
    Contains interface.h and one C file for each anaglyph method.

---------------------
External Dependencies
---------------------
Required:
  * gcc
  * OpenCV
  * make

Optional:
  * OpenMP - Used for multithreading.  To turn this off, see the multithreading
             option in settings.mk
  * lapack - Library that levmar uses for faster non-linear solvers.  Read in
             levmar's README.txt about how to compile levmar with lapack.

Note that the video and image files that can be read and written are dependent
on what libraries are available to OpenCV.

Mac Specifics
`````````````
MacPorts can be used to install all of the above except for levmar and lapack.
Installing OpenCV will also install FFMpeg support and OpenMP.  For more
details on Mac installation, read OpenCV's documentation:

http://opencv.willowgarage.com/wiki/Mac_OS_X_OpenCV_Port

---------
Compiling
---------

First you need to compile levmar, which is used as a non-linear least squares
solver.  Go into the levmar/ directory, which is inside the root of the
repository.  Next, run the "make" command inside this directory.  This should
compile levmar with some sane defaults.  If needed, more details about
compiling levmar can be found in levmar's "README.txt".

Levmar is only used for the cielab method.  In the future, an option may be
added to make levmar optional.

Next you need to create your settings file.  Change directories back to the
root of the repository.  Copy "settings.mk.default" to "settings.mk".  Open
"settings.mk" and read through the options.  You will need to change some of
these before compiling.  Read the comments in settings.mk for details.

To compile, run the "make" command inside the root of the repository.  The
executables generated, one for each method, will be placed in the bin/
directory.

-------
Running
-------
Run one of the executables in bin/<method_name> and pass in as arguments a left
eye image and a right eye image as arguments.  There are some sample anaglyph
images in the images/ directory, with the files clearly labeled as
corresponding left and right eye images.

For more help on usage, execute "bin/<method_name> --help".

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

