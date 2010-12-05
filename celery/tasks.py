from celery.decorators import task
import shlex, subprocess

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
