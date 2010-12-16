from celery.decorators import task
import shlex, subprocess
import cv
import memcache

@task
def MemcacheAnaglyphTask(**kwargs):
    mc = memcache.Client(['152.14.17.125:11211'], debug=1)
    frames_per_task = kwargs['frames_per_task']
    start_frame = int(kwargs['frame'])
    for i in xrange(frames_per_task):
        frame_id = (start_frame + i).__str__()
        frame = mc.get(frame_id)
        cv_im = cv.CreateImage(kwargs['size'], cv.IPL_DEPTH_8U, 3)
        cv.SetData(cv_im, frame, kwargs['size'][0]*3)
        cv.SaveImage("/home/dkliban/anaglyph/images/tmp.bmp", cv_im)
        combined_image = kwargs['combined_image'] + frame_id + "C.bmp"
        args = ["/home/dkliban/anaglyph/bin/cielab", "/home/dkliban/anaglyph/images/tmp.bmp", kwargs["right_image"], "-o", combined_image]
        p = subprocess.Popen(args, stdout=subprocess.PIPE).communicate()[0]
        print args
    mc.disconnect_all()
    return kwargs

@task
def AnaglyphFrameTask(**kwargs):
    frame = kwargs['frame']
    cv_im = cv.CreateImage(kwargs['size'], cv.IPL_DEPTH_8U, 3)
    cv.SetData(cv_im, frame, kwargs['size'][0]*3)
    cv.SaveImage("/home/dkliban/anaglyph/images/tmp.bmp", cv_im)
    args = ["/home/dkliban/anaglyph/bin/cielab", "/home/dkliban/anglyph/images/tmp.bmp", kwargs["right_image"], "-o", kwargs["combined_image"]]
    p = subprocess.Popen(args, stdout=subprocess.PIPE).communicate()[0]
    print args
    return kwargs

@task
def McAllisterAnaglyphTask(**kwargs):
    args = ["/home/dkliban/anaglyph/bin/cielab", kwargs["left_image"], kwargs["right_image"], "-o", kwargs["combined_image"]]
    p = subprocess.Popen(args, stdout=subprocess.PIPE).communicate()[0]
    print args
    return kwargs

@task
def McAllisterVideoTask(**kwargs):
    print 'handling task: Frame %(frame)s, of %(combined_name)s' % kwargs
    args = ["/home/dkliban/anaglyph/bin/cielab", kwargs["left_video"], kwargs["right_video"], "-o", kwargs["combined_name"]+kwargs["frame"]+".png", "-v", "-f", kwargs["frame"]]
    p = subprocess.Popen(args, stdout=subprocess.PIPE).communicate()[0]
    print args
    return kwargs
