
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include <cv.h>
#include <highgui.h>

#include "methods/interface.h" // All method_* are defined here

/**
 * Combines a left and right eye image into an anaglyph.
 */
IplImage* combine_stereo(IplImage* left_eye, IplImage* right_eye)
{
    IplImage* stereo = cvCreateImage(cvSize(left_eye->width, left_eye->height), IPL_DEPTH_8U, 3);

    for (int x=0; x<stereo->width; x++)
    {
        for (int y=0; y<stereo->height; y++)
        {
            CvScalar left_pixel = cvGet2D(left_eye, y, x);
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
        printf("The left-eye and right-eye arguments must be image files, avi files, or numbers\n");
        printf("representing a camera index.\n");
        printf("\n");
        printf("    --output <file_name>\n");
        printf("    -o <file_name>         Specify output file.  If none is specified, output\n");
        printf("                           will be shown in a window.\n");
        printf("    --video\n");
        printf("    -v                     Indicates that the left and right eye image files \n");
        printf("                           given are video files, or camera indexes.\n");
        exit(1);
}

CvCapture* open_video(char* str) {

        CvCapture* capture;
        if (strlen(str)==1 && isdigit(str[0]))
        {
            capture = cvCaptureFromCAM(atoi(str));
        } else {
            capture = cvCaptureFromFile(str);
        }
        return capture;

}

int main(int argc, char** argv)
{
    // Parse Arguments
    char* left_eye_string = NULL;
    char* right_eye_string = NULL;
    char* output_file = NULL;
    bool files_are_video = false;
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

        // Video
        } else if (strcmp(argv[i], "-v") == 0 ||
                   strcmp(argv[i], "--video") == 0) {
            files_are_video = true;

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
        printf("\nError: Left and Right eye files not specified.\n\n");
        usage_message(argv[0]);
    }

    if (output_file == NULL) {
        cvNamedWindow("Stereo", CV_WINDOW_AUTOSIZE);
    }

    if (files_are_video) {

        CvCapture* left_eye_capture = open_video(left_eye_string);
        CvCapture* right_eye_capture = open_video(right_eye_string);

        // Check dimmensions
        int left_width = cvGetCaptureProperty(left_eye_capture,
            CV_CAP_PROP_FRAME_WIDTH);
        int left_height = cvGetCaptureProperty(left_eye_capture,
            CV_CAP_PROP_FRAME_HEIGHT);
        int right_width = cvGetCaptureProperty(right_eye_capture,
            CV_CAP_PROP_FRAME_WIDTH);
        int right_height = cvGetCaptureProperty(right_eye_capture,
            CV_CAP_PROP_FRAME_HEIGHT);
        if (left_width != right_width || left_height != right_height) {
            printf("\nError: Right and left eye image must have the same dimmensions!\n\n");
            exit(1);
        }

        CvVideoWriter* output_writer;
        if (output_file != NULL) {
            output_writer = cvCreateVideoWriter(output_file,
                CV_FOURCC('M','J','P','G'), // Codec
                20, // fps
                cvSize(left_width, left_height),
                1 // is_color
            );
        }

        if (!left_eye_capture) {
            printf("Could not open left eye video: %s\n", left_eye_string);
            exit(1);
        }
        if (!right_eye_capture) {
            printf("Could not open right eye video: %s\n", right_eye_string);
            exit(1);
        }

        method_init();

        bool breakflag = false;
        while (true) {
            IplImage* left_eye = cvQueryFrame(left_eye_capture);
            IplImage* right_eye = cvQueryFrame(right_eye_capture);
            if (!left_eye || !right_eye) { break; }
            printf("Calculating Frame: %d\n", (int)cvGetCaptureProperty(left_eye_capture, CV_CAP_PROP_POS_FRAMES)+1);
            IplImage* stereo = combine_stereo(left_eye, right_eye);

            if (output_file == NULL) {
                cvShowImage("Stereo", stereo);
            } else {
                cvWriteFrame(output_writer, stereo);
            }

            int key = cvWaitKey(10);
            if ( (char) key == 27) { // Esc to exit
                breakflag = true;
                break;
            }
            switch( (char) key) {
                // Keyboard Commands
            }
            cvFree(&stereo);

        }

        if (output_file != NULL) {
            cvReleaseVideoWriter(&output_writer);
        }
        method_free();
        if (output_file == NULL && !breakflag) {
            cvWaitKey(-1);
        }

    } else {

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
        if (left_eye->width != right_eye->width ||
            left_eye->height != right_eye->height)
        {
            printf("\nError: Right and left eye image must have the same dimmensions!\n\n");
        }

        method_init();

        IplImage* stereo = combine_stereo(left_eye, right_eye);
        if (output_file == NULL) {
            cvShowImage("Stereo", stereo);
        } else {
            #if CV_MAJOR_VERSION>=2
                cvSaveImage(output_file, stereo, 0);
            #else
                cvSaveImage(output_file, stereo);
            #endif
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

    }

    return 0;
}
