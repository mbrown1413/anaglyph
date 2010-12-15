from tasks import MemcacheAnaglyphTask, McAllisterVideoTask, McAllisterAnaglyphTask, AnaglyphFrameTask
import ctypes
from celery.task.sets import TaskSet
from celery.task.control import Control
from celery.app import app_or_default
import cv
import sys
import os.path
from time import time
from math import ceil
import memcache

sys.path.append("/usr/local/lib/python2.6/site-packages")


def write_frame(**params):
    video = params['video']
    frame = cv.QueryFrame(video)
    cv.SaveImage("%(name)s/%(frame)s.bmp" % params, frame)

def memcache_frame(**params):
    mc = memcache.Client(['localhost:11211'], debug=1)
    video = params['video']
    frame = cv.QueryFrame(video).tostring().__str__()
    mc.set("%(frame)s" % params, frame)
    #r = mc.get("%(frame)s" % params)
    #if r is None:
    #    print "Couldn't retrieve from memcache"

def dispatch_frames(**params):
    batch_size = params['batch_size']
    start_id = params['start_id']

    the_subtasks = []
    for i in xrange(start_id, start_id+batch_size):
        the_subtasks.append(McAllisterAnaglyphTask.subtask(left_image="%(name)s/%(frame)s.bmp" % {'name': params['name'],'frame':i},right_image="-s",combined_image="%(name)s/%(frame)sC.bmp" % {'name': params['name'], 'frame':i}))
    job = TaskSet(tasks=the_subtasks)
    results = job.apply_async();
    return results

def dispatch_full_frames(**params):
    batch_size = params['batch_size']
    start_id = params['start_id']
    video = params['video']
    size = params['size']

    the_subtasks = []
    data = {}
    start_time = time()
    for i in xrange(start_id, start_id+batch_size):
        cvFrame = cv.QueryFrame(video)
        frame = cvFrame.tostring().__str__()
        #cv_im = cv.CreateImage((cvFrame.width,cvFrame.height), cvFrame.depth, cvFrame.nChannels)
        #cv_im = cv.CreateImageHeader(size, cv.IPL_DEPTH_8U, 3)
        #cv.SetData(cv_im, frame, cvFrame.nChannels*size[0])
        #cv.SaveImage("/home/dkliban/anaglyph/images/tmp.bmp", cv_im)   
        the_subtasks.append(AnaglyphFrameTask.subtask(size=size, frame=frame, left_image="%(name)s/%(frame)s.bmp" % {'name': params['name'],'frame':i},right_image="-s",combined_image="%(name)s/%(frame)sC.bmp" % {'name': params['name'], 'frame':i}))
    job = TaskSet(tasks=the_subtasks)
    results = job.apply_async();
    end_time = time()
    data['batch_size'] = batch_size
    data['average_dispatch'] = (end_time - start_time)/batch_size
    return data

def dispatch_memcache_frames(**params):
    batch_size = params['batch_size']
    start_id = params['start_id']
    video = params['video']
    size = params['size']

    the_subtasks = []
    data = {}
    start_time = time()
    for i in xrange(start_id, start_id+batch_size):
        memcache_frame(video=video, frame=i, name=params['name']) 
        the_subtasks.append(MemcacheAnaglyphTask.subtask(size=size, frame=i, name=params['name'],left_image="%(name)s/%(frame)s.bmp" % {'name': params['name'],'frame':i},right_image="-s",combined_image="%(name)s/%(frame)sC.bmp" % {'name': params['name'], 'frame':i}))
    job = TaskSet(tasks=the_subtasks)
    results = job.apply_async();
    end_time = time()
    data['batch_size'] = batch_size
    data['average_dispatch'] = (end_time - start_time)/batch_size
    return data

def revoke_all_tasks():
    c = Control(app_or_default())
    print c.discard_all()

    

def write_and_dispatch(**params):
    name = params['name']
    batch_size = params['batch_size']
    video = params['video']
    start_id = params['start_id']
    data = {} 
    start_time = time()
    for i in xrange(start_id, start_id+batch_size):
        write_frame(video=video, name=name, frame=i)
    end_time = time()
    data['average_write'] = (end_time - start_time) / batch_size
    
    start_time = time()
    data['result'] = dispatch_frames(name=name, batch_size=batch_size, start_id=start_id)
    end_time = time()
    data['average_dispatch'] = (end_time - start_time) / batch_size
    data['batch_size'] = batch_size
    #print "%(batch_size)s, %(average_write)s, %(average_dispatch)s;" % data
    return data

def dispatch_video_test(**params):
    CV_CAP_PROP_FRAME_COUNT = 7
    name = params['combined_name']
    video = cv.CaptureFromFile(params['left_video'])
    number_frames = cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_COUNT)

    print number_frames

    width = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_WIDTH))
    height = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_HEIGHT))
    for i in xrange(200):
        cv.QueryFrame(video)
    frame = 200
    for i in xrange(1,2):
        data = {}
        data['batch_size'] = i
        data['average_dispatch'] = 0
        data['average_write'] = 0
        for j in xrange(100):
            datapoint = write_and_dispatch(name=name, batch_size=i, video=video, start_id=frame)
            frame = frame + i
            data['average_dispatch'] = data['average_dispatch'] + datapoint['average_dispatch']
            data['average_write'] = data['average_write'] + datapoint['average_write']
        data['average_dispatch'] = data['average_dispatch'] / 100
        data['average_write'] = data['average_write'] / 100
        print "%(batch_size)s, %(average_write)s, %(average_dispatch)s, " % data

def dispatch_video_frames(**params):
    CV_CAP_PROP_FRAME_COUNT = 7
    name = params['combined_name']
    video = cv.CaptureFromFile(params['left_video'])
    number_frames = cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_COUNT)

    print number_frames

    width = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_WIDTH))
    height = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_HEIGHT))
    size = (width, height)
    for i in xrange(200):
        cv.QueryFrame(video)
    frame = 200
    for i in xrange(1,10):
        data = {}
        data['batch_size'] = i
        data['average_dispatch'] = 0
        for j in xrange(100):
            datapoint = dispatch_memcache_frames(size=size, name=name, batch_size=i, video=video, start_id=frame)
            frame = frame + i
            data['average_dispatch'] = data['average_dispatch'] + datapoint['average_dispatch']
        data['average_dispatch'] = data['average_dispatch'] / 100
        print "%(batch_size)s, %(average_dispatch)s, " % data

def dispatch_video(**params):
    CV_CAP_PROP_FRAME_COUNT = 7

    #"/home/michael/anaglyph/sw-L/left.avi"

    video = cv.CaptureFromFile(params['left_video'])
    print params['left_video']
    number_frames = cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_COUNT)

    print number_frames

    width = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_WIDTH))
    height = int(cv.GetCaptureProperty(video, cv.CV_CAP_PROP_FRAME_HEIGHT))

    counter = 0
    for i in xrange(100):
        counter = counter + 1
        frame = cv.QueryFrame(video)
        cv.SaveImage("%(name)s/%(fn)s.bmp" % {'name':params['combined_name'], 'fn':i}, frame)
        print "writing out frame %(i)s ... " % {'i':i}
    
    output = cv.CreateVideoWriter(params['combined_name']+".avi", cv.FOURCC('M','J','P','G'), 20, (width/2, height), 1)
    the_subtasks = []
    for i in xrange(100):
        the_subtasks.append(McAllisterAnaglyphTask.subtask(left_image="%(name)s/%(fn)s.bmp" % {'name':params['combined_name'], 'fn':i},right_image="-s",combined_image="%(name)s/%(fn)sC.bmp" % {'name':params['combined_name'], 'fn':i}))
    job = TaskSet(tasks=the_subtasks)
    results = job.apply_async()
    #while(not results.ready()):
    #    print "waiting ..." 
    for i in xrange(100):
        while(not os.path.isfile("%(name)s/%(fn)sC.bmp" % {'name':params['combined_name'], 'fn':i})):
            pass
        next_frame = cv.LoadImage("%(name)s/%(fn)sC.bmp" %   
                {'name':params['combined_name'], 'fn':i})
        print "%(name)s/%(fn)sC.bmp" % {'name':params['combined_name'], 'fn':i}
        cv.WriteFrame(output, next_frame)

    #current = 0
    #while (results):
    #    if (results[0].ready()):
    #        task_result = results.popleft().result
    #        print '%(combined_name)s%(frame)s.png' % task_result
    #        next_frame = cv.LoadImage('%(combined_name)s%(frame)s.png' % task_result)
    #        cv.WriteFrame(output, next_frame)
    #        current = current + 1


def build_video(**params):
    number_frames = params['number_frames']
    frame_rate = params['frame_rate']
    width = params['size'][0]
    height = params['size'][1]

    output = cv.CreateVideoWriter(params['combined_name']+".avi", cv.FOURCC('M','J','P','G'), frame_rate, (width/2, height), 1)
    for i in xrange(number_frames):
        while(not os.path.isfile("%(name)s/%(fn)sC.bmp" % {'name':params['combined_name'], 'fn':i})):
            pass
        next_frame = cv.LoadImage("%(name)s/%(fn)sC.bmp" %   
                {'name':params['combined_name'], 'fn':i})
        print "%(name)s/%(fn)sC.bmp" % {'name':params['combined_name'], 'fn':i}
        cv.WriteFrame(output, next_frame)
    
        
def test_image():
    dispatch_image(left_image="/home/dkliban/anaglyph/images/david-left.jpg", right_image="/home/dkliban/anaglyph/images/david-right.jpg", combined_image="/home/dkliban/anaglyph/davidddd.png")

def test_video():
    dispatch_video_frames(left_video="/home/dkliban/anaglyph/videos/Ray3D.avi", right_video="-s", combined_name="/home/dkliban/dkliban_research/storage/ray3Danaglyph")

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
