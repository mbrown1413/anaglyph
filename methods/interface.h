/** interface.h
 * Defines the interface which anaglyph methods implement.
 */

#ifndef __ANAGLYPH_METHOD_INTERFACE_H
#define __ANAGLYPH_METHOD_INTERFACE_H

/** combine_pixels
 * Takes in two pixel values for the left and the right eye.  Outputs the
 * combination of the two.  The combination method depends on which method is
 * being used.
 *
 * The use of CvScalar is a bit confusing.  All of the CvScalars have four
 * elements, but only 3 of them are used.  Also, OpenCV likes to use BGR
 * instead of RGB, like normal people.  So to clarify everything:
 * double left_blue  = left_pixel.val[0];
 * double left_green = left_pixel.val[1];
 * double left_red   = left_pixel.val[2];
 */
CvScalar combine_pixels(CvScalar left_pixel, CvScalar right_pixel);

#endif
