from celery.decorators import task
import shlex, subprocess

@task
def McAllisterAnaglyphTask(**kwargs):
    #print 'handling task: Frame %(0)s, of %(combined_prefix)s' % kwargs
    args = ["/home/dkliban/anaglyph/bin/cielab", kwargs["left_image"], kwargs["right_image"], "-o", kwargs["combined_image"]]
    try:
        p = subprocess.Popen(args)
        subprocess.Popen.wait(p)
        return 1
    except Exception:
        return 0

@task
def McAllisterVideoTask(**kwargs):
    print 'handling task: Frame %(frame)s, of %(combined_name)s' % kwargs
    args = ["/home/dkliban/anaglyph/bin/cielab", kwargs["left_video"], kwargs["right_video"], "-o", kwargs["combined_name"]+kwargs["frame"]+".png", "-v", "-f", kwargs["frame"]]
    p = subprocess.Popen(args, stdout=subprocess.PIPE).communicate()[0]
    print args
    return kwargs
