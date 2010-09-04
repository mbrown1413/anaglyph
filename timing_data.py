
NUMBER_OF_RUNS = 5 # Every image gets calculated this many times
images = [ # ("Pretty name", "left eye image", "right eye image")
    ("Jupiter", "images/jupiter-left.jpg", "images/jupiter-right.jpg"),
    ("David", "images/david-left.jpg", "images/david-right.jpg"),
]

from subprocess import Popen, PIPE

print "Make sure main.c is set to exit after generating the first image."
for image in images:
    print "\n%s:" % image[0]
    for i in xrange(NUMBER_OF_RUNS):
        command = [
            "time",
            "-f", '%e', # output total "real" time
            "./bin/mcallister",
            image[1], image[2],
        ]
        p = Popen(command, stdout=PIPE)
        p.wait()
print ""
