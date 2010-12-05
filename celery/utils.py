from tasks import McAllisterVideoTask, McAllisterAnaglyphTask
import ctypes
from celery.task.sets import TaskSet
import cv
import sys

sys.path.append("/usr/local/lib/python2.6/site-packages")

def testing(**params):
    CV_CAP_PROP_FRAME_COUNT = 7

    #"/home/michael/anaglyph/sw-L/left.avi"

    video = cv.CaptureFromFile(params['left_video'])
    print params['left_video']
    number_frames = cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_COUNT)

    print number_frames

    width = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_WIDTH))
    height = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_HEIGHT))

    for i in xrange(number_frames):
        frame = cv.QueryFrame(video)
        cv.SaveImage("%(name)s/%(fn)s.jpg" % {'name':params['combined_name'], 'fn':i}, frame)


def dispatch_video(**params):
    CV_CAP_PROP_FRAME_COUNT = 7

    #"/home/michael/anaglyph/sw-L/left.avi"

    video = cv.CaptureFromFile(params['left_video'])
    print params['left_video']
    number_frames = cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_COUNT)

    print number_frames

    width = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_WIDTH))
    height = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_HEIGHT))

    for i in xrange(200):
        frame = cv.QueryFrame(video)
        cv.SaveImage("%(name)s/%(fn)s.jpg" % {'name':params['combined_name'], 'fn':i}, frame)
    
    output = cv.CreateVideoWriter(params['combined_name']+".avi", cv.FOURCC('M','J','P','G'), 30, (width/2, height), 1)
    the_subtasks = []
    for i in xrange(200):
        the_subtasks.append(McAllisterAnaglyphTask.subtask(left_image="%(name)s/%(fn)s.jpg" % {'name':params['combined_name'], 'fn':i},right_image="-s",combined_image="%(name)s/%(fn)sC.jpg" % {'name':params['combined_name'], 'fn':i}))
    job = TaskSet(tasks=the_subtasks)
    results = job.apply_async()

    for r in results.iterate():
        next_frame = cv.LoadImage('%(combined_image)s' % r)
        print r
        cv.WriteFrame(output, next_frame)

    #current = 0
    #while (results):
    #    if (results[0].ready()):
    #        task_result = results.popleft().result
    #        print '%(combined_name)s%(frame)s.png' % task_result
    #        next_frame = cv.LoadImage('%(combined_name)s%(frame)s.png' % task_result)
    #        cv.WriteFrame(output, next_frame)
    #        current = current + 1

        
def test_image():
    dispatch_image(left_image="/home/dkliban/anaglyph/images/david-left.jpg", right_image="/home/dkliban/anaglyph/images/david-right.jpg", combined_image="/home/dkliban/anaglyph/davidddd.png")

def test_video():
    dispatch_video(left_video="/home/dkliban/anaglyph/videos/Ray3D.avi", right_video="-s", combined_name="/home/dkliban/dkliban_research/storage/ray3Danaglyph")

def test_testing():
    testing(left_video="/home/dkliban/anaglyph/videos/Ray3D.avi", right_video="-s",          combined_name="/home/dkliban/dkliban_research/storage/ray3Danaglyph")

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
