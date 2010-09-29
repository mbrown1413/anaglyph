/** interface.h
 * Defines the interface which anaglyph methods implement.
 */

#ifndef __ANAGLYPH_METHOD_INTERFACE_H
#define __ANAGLYPH_METHOD_INTERFACE_H

/**
 * method_init
 * Any sort of initialization that the method needs to do should be done here.
 *
 * This is called only once, before any calculations are done.
 *
 * This method is not required to do anything, but it is required to be defined
 * inside the method file.
 */ 
void method_init(void);

/** method_combine_pixels
 * Takes in two pixel values for the left and the right eye.  Outputs the
 * combination of the two.
 *
 * This is called for every pixel in every anaglyph calculation.
 *
 * The use of CvScalar is a bit confusing.  All of the CvScalars have four
 * elements, but only 3 of them are used.  Also, OpenCV likes to use BGR
 * instead of RGB, like normal people.  So to clarify everything:
 * float left_blue  = left_pixel.val[0];
 * float left_green = left_pixel.val[1];
 * float left_red   = left_pixel.val[2];
 */
CvScalar method_combine_pixels(CvScalar left_pixel, CvScalar right_pixel);

/**
 * method_free
 * Anything that needs to be done all calculations are done, such as feeing
 * memory.
 *
 * This is called only once, after all calculations are done.
 *
 * This method is not required to do anything, but it is required to be defined
 * inside the method file.
 */ 
void method_free(void);

#endif
