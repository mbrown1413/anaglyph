from tasks import McAllisterVideoTask, McAllisterAnaglyphTask
import ctypes
from celery.task.sets import TaskSet
import cv
import sys
import opencv

sys.path.append("/usr/local/lib/python2.6/site-packages")


def dispatch_video(**params):
    opencv = ctypes.cdll.LoadLibrary("libopencv.so")
    left_video = opencv.cvCreateFileCapture(params['left_video'])
    print params['left_video']
    left_number_frames = opencv.cvGetCaptureProperty(left_video, CV_CAP_PROP_FRAME_COUNT)
    height = opencv.cvGetCaptureProperty(left_video, CV_CAP_PROP_FRAME_HEIGHT)
    width = opencv.cvGetCaptureProperty(left_video, CV_CAP_PROP_FRAME_WIDTH)
    print opencv.cvGetCaptureProperty(left_video, 0)
    print opencv.cvGetCaptureProperty(left_video, 1)
    print opencv.cvGetCaptureProperty(left_video, 2)
    print opencv.cvGetCaptureProperty(left_video, 3)
    print opencv.cvGetCaptureProperty(left_video, 4)
    print opencv.cvGetCaptureProperty(left_video, 5)
    print opencv.cvGetCaptureProperty(left_video, 6)
    print opencv.cvGetCaptureProperty(left_video, 7)
    output = opencv.cvCreateVideoWriter(params['combined_name']+".avi", opencv.CV_FOURCC('M','J','P','G'), 20, opencv.CvSize(width/2, height), True)

    #"/home/michael/anaglyph/sw-L/left.avi"

    the_subtasks = []
    results = {}
    #for i in xrange(left_number_frames):
    for i in xrange(2500):
        results[i] = McAllisterVideoTask.delay(left_video=params['left_video'],right_video="-s",frame=str(i),combined_name=params['combined_name'])
    
    current = 0
    while (results):
        if (results[current].ready()):
            task_result = results.pop(current).result
            print '%(combined_name)s%(frame)s.png' % task_result
            next_frame = opencv.cvLoadImage('%(combined_name)s%(frame)s.png' % task_result)
            opencv.cvWriteFrame(output, next_frame)
            current = current + 1

        
def test_image():
    dispatch_image(left_image="/home/dkliban/anaglyph/images/david-left.jpg", right_image="/home/dkliban/anaglyph/images/david-right.jpg", combined_image="/home/dkliban/anaglyph/davidddd.png")

def test_video():
    dispatch_video(left_video="/home/dkliban/anaglyph/videos/Ray3D.avi", right_video="-s", combined_name="/home/dkliban/dkliban_research/storage/ray3Danaglyph")

def dispatch_image(**params):
    the_subtasks = []
    the_subtasks.append(McAllisterAnaglyphTask.subtask(**params))
    the_subtasks.append(McAllisterAnaglyphTask.subtask(**params))
    the_subtasks.append(McAllisterAnaglyphTask.subtask(**params))
    the_subtasks.append(McAllisterAnaglyphTask.subtask(**params))
    the_subtasks.append(McAllisterAnaglyphTask.subtask(**params))
    job = TaskSet(tasks=the_subtasks)
    result = job.apply_async()
    return result

class NumberFramesError(Exception):
    def __init__(self, value):
         self.value = value
    
    def __str__(self):
        return repr(self.value)
