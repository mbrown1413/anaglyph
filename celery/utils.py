from tasks import McAllisterVideoTask, McAllisterAnaglyphTask
import ctypes
from celery.task.sets import TaskSet

CV_CAP_PROP_POS_MSEC = 0 
CV_CAP_PROP_POS_FRAMES = 1 
CV_CAP_PROP_POS_AVI_RATIO = 2 
CV_CAP_PROP_FRAME_WIDTH = 3 
CV_CAP_PROP_FRAME_HEIGHT = 4 
CV_CAP_PROP_FPS = 5 
CV_CAP_PROP_FOURCC = 6 
CV_CAP_PROP_FRAME_COUNT = 7 

def dispatch_video(**params):

    #"/home/michael/anaglyph/sw-L/left.avi"

    highgui = ctypes.cdll.LoadLibrary("libhighgui.so")
    left_video = highgui.cvCreateFileCapture(params['left_video'])
    print params['left_video']
    left_number_frames = highgui.cvGetCaptureProperty(left_video, CV_CAP_PROP_FRAME_COUNT)
    height = highgui.cvGetCaptureProperty(left_video, CV_CAP_PROP_FRAME_HEIGHT)
    width = highgui.cvGetCaptureProperty(left_video, CV_CAP_PROP_FRAME_WIDTH)
    print highgui.cvGetCaptureProperty(left_video, 0)
    print highgui.cvGetCaptureProperty(left_video, 1)
    print highgui.cvGetCaptureProperty(left_video, 2)
    print highgui.cvGetCaptureProperty(left_video, 3)
    print highgui.cvGetCaptureProperty(left_video, 4)
    print highgui.cvGetCaptureProperty(left_video, 5)
    print highgui.cvGetCaptureProperty(left_video, 6)
    print highgui.cvGetCaptureProperty(left_video, 7)
    output = highgui.cvCreateVideoWriter(params['combined_name']+".avi", CV_FOURCC('M','J','P','G'), 20, CvSize(width/2, height), True)
    the_subtasks = []
    results = {}
    #for i in xrange(left_number_frames):
    for i in xrange(500):        
	results[i] = McAllisterVideoTask.delay(left_video=params['left_video'],right_video="-s",frame=str(i),      
combined_name=params['combined_name'])

    current = 0    
    while (results):
        if (results[current].ready()):
            task_result = results.pop(current).result
            print task_result
            #print '%(combined_name)s%(frame)s.png' % task_result
            next_frame = cv.LoadImage('%(combined_name)s%(frame)s.png' % task_result)
            cv.WriteFrame(output, next_frame)
            current = current + 1

def CV_FOURCC( c1, c2, c3, c4 ) :
    return (((ord(c1))&255)             \
                + (((ord(c2))&255)<<8)  \
                + (((ord(c3))&255)<<16) \
                + (((ord(c4))&255)<<24))

class CvSize(ctypes.Structure):
    _fields_ = [("width", ctypes.c_int),
                ("height", ctypes.c_int)]


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
