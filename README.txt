
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
* gcc
* OpenCV
* make

If you want to read video, you will need to compile OpenCV with FFMPEG support.

Mac Specifics
`````````````
MacPorts can be used to install all of the above.  FFMPEG support will be built
into OpenCV as well.  For more details on Mac installation, read OpenCV's
documentation:

http://opencv.willowgarage.com/wiki/Mac_OS_X_OpenCV_Port

---------------------
Compiling and Running
---------------------
To compile, type the "make" command inside the root of the repository.  This
reads the makefile located in the file "makefile" to read the compiler commands
and run them.  The executables generated, one for each method, will be placed
in the bin/ directory.

You may have to change the OPENCV_FLAGS variable inside "makefile" if your
compiler can't find the OpenCV header files or the OpenCV object files.  The -I
flag specifies where to look for include files.  The -L flag specifies
where to look for object files.

Mac Specifics
`````````````
When using MacPorts, in my experience, the include files are located at
"/opt/local/include" and the object files are located at "/opt/local/lib".

-----------------
Creating a Method
-----------------
If you want to create your own anaglyph method, create a file named
"methods/<method_name>.c".  This file will automatically be compiled after
following the steps in the "Compiling and Running" section and the executable
file will be put in bin/.

Look in methods/interface.h.  This is the exact set of function prototypes that
you need to have in your .c file.  Read methods/interface.h for documentation
on exactly what these functions do, and when they are called.
