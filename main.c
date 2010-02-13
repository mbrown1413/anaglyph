#include <ctype.h>
#include <time.h>
#include <stdio.h>

#include <cv.h>
#include <highgui.h>

#include "methods/interface.h"

IplImage* combine_stereo(IplImage* left_eye, IplImage* right_eye, int offset_x)
{
    IplImage* stereo = cvCreateImage(cvSize(left_eye->width, left_eye->height), IPL_DEPTH_8U, 3);

    for (int x=0; x<stereo->width-1; x++)
    {
        for (int y=0; y<stereo->height-1; y++)
        {
            //TODO: Optimize pixel setting/getting
            //printf("%d, %d\n", x, y);
            CvScalar left_pixel;
            if (x+offset_x < stereo->width-1 && x+offset_x>=0) {
                left_pixel = cvGet2D(left_eye, y, x+offset_x);
            } else {
                left_pixel = cvScalar(0,0,0,0);
            }
            CvScalar right_pixel = cvGet2D(right_eye, y, x);
            CvScalar stereo_pixel = combine_pixels(left_pixel, right_pixel);
            //printf("%f %f %f\n", left_pixel.val[0], left_pixel.val[1], left_pixel.val[2]);
            cvSet2D(stereo, y, x, stereo_pixel);
        }
    }

    return stereo;
}

int main(int argc, char** argv)
{
    int offset_x = 127; // Added offset to left eye
    int previous_offset_x = -1;
    cvNamedWindow("Stereo", CV_WINDOW_AUTOSIZE);
    cvCreateTrackbar("x Offset", "Stereo", &offset_x, 255, NULL);

    /*
    char left_eye_image_name[100];
    char right_eye_image_name[100];
    sprintf(left_eye_image_name, "images/%s-left.jpg", argv[1]);
    sprintf(right_eye_image_name, "images/%s-right.jpg", argv[1]);
    IplImage* left_eye = cvLoadImage(left_eye_image_name, 0);
    IplImage* right_eye = cvLoadImage(right_eye_image_name, 0);
    */
    //TODO: Handle incorrect arguments nicely
    IplImage* left_eye = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
    IplImage* right_eye = cvLoadImage(argv[2], CV_LOAD_IMAGE_COLOR);
    IplImage* stereo = NULL;

    unsigned int frames = 0;
    int key;
    for (frames=1; 1; frames++)
    {
        if (offset_x != previous_offset_x)
        {
            cvFree(&stereo); // Doesn't do anything the first time through
            stereo = combine_stereo(left_eye, right_eye, offset_x-127);
            cvShowImage("Stereo", stereo);
            previous_offset_x = offset_x;
        }
        key = cvWaitKey(100);
        if ( (char) key == 27) { // Esc to exit
            break;
        }
        switch( (char) key) {
            // Keyboard Commands
        }

    }

    return 0;
}
