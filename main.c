#include <ctype.h>
#include <time.h>
#include <stdio.h>

#include <cv.h>
#include <highgui.h>

#include "methods/interface.h" // All method_* are defined here

/**
 * Combines a left and right eye image into an anaglyph.
 */
IplImage* combine_stereo(IplImage* left_eye, IplImage* right_eye, int offset_x)
{
    IplImage* stereo = cvCreateImage(cvSize(left_eye->width, left_eye->height), IPL_DEPTH_8U, 3);

    for (int x=0; x<stereo->width-1; x++)
    {
        for (int y=0; y<stereo->height-1; y++)
        {
            //TODO: Optimize pixel setting/getting
            CvScalar left_pixel;
            if (x+offset_x < stereo->width-1 && x+offset_x>=0) {
                left_pixel = cvGet2D(left_eye, y, x+offset_x);
            } else {
                left_pixel = cvScalar(0,0,0,0);
            }
            CvScalar right_pixel = cvGet2D(right_eye, y, x);
            CvScalar stereo_pixel = method_combine_pixels(
                left_pixel, right_pixel
            );
            cvSet2D(stereo, y, x, stereo_pixel);
        }
    }

    return stereo;
}

void usage_message(char* program_name) {
        printf("Usage: %s [options] <left-eye> <right-eye>\n", program_name);
        printf("The left-eye and right-eye arguments as of now, must be image files.  In the future, ffmpeg, avi and webcam support will be added.\n");
        printf("\n");
        printf("    -o <file_name>         Specify output file.  If none is specified, output\n                           will be shown in a window.\n");
        exit(1);
}

int main(int argc, char** argv)
{
    int offset_x = 127; // Added offset to left eye.  127 is center
    int previous_offset_x = -1;

    // Parse Arguments
    char* left_eye_string = NULL;
    char* right_eye_string = NULL;
    char* output_file = NULL;
    for (int i=1; i<argc; i++) {

        // Output Image
        if (strcmp(argv[i], "-o") == 0) {
            if (i == argc-1) {
                printf("\nError: -o takes a filename as an argument.\n\n");
                usage_message(argv[0]);
            }
            output_file = argv[i+1];
            i++; // Skip next arg

        // Help
        } else if (strcmp(argv[i], "-h") == 0 ||
                   strcmp(argv[i], "--help") == 0)
        {
            usage_message(argv[0]);

        // Left or right eye image
        } else {
            if (left_eye_string == NULL) {
                left_eye_string = argv[i];
            } else if (right_eye_string == NULL) {
                right_eye_string = argv[i];
            } else {
                printf("\nError: Too many arguments specified.\n\n");
                usage_message(argv[0]);
            }
        }

    }
    if (!left_eye_string || !right_eye_string) {
        printf("\nError: Left and Right eye images not specified.\n\n");
        usage_message(argv[0]);
    }

    if (output_file == NULL) {
        cvNamedWindow("Stereo", CV_WINDOW_AUTOSIZE);
    }

    IplImage* left_eye = cvLoadImage(left_eye_string, CV_LOAD_IMAGE_COLOR);
    IplImage* right_eye = cvLoadImage(right_eye_string, CV_LOAD_IMAGE_COLOR);

    if (left_eye == NULL) {
        printf("Could not open left eye image.\n");
        exit(1);
    }
    if (right_eye == NULL) {
        printf("Could not open right eye image.\n");
        exit(1);
    }

    method_init();

    IplImage* stereo = combine_stereo(left_eye, right_eye, 0);
    if (output_file == NULL) {
        cvShowImage("Stereo", stereo);
    } else {
        cvSaveImage(output_file, stereo, 0);
    }

    cvFree(&stereo);
    method_free();

    if (output_file == NULL) {
        while (1) {
            int key = cvWaitKey(-1);
            if ( (char) key == 27) { // Esc to exit
                break;
            }
            switch( (char) key) {
                // Keyboard Commands
            }
        }
    }

    return 0;
}
