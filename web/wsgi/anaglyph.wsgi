import os
import sys

def get_parent_directory():
    return os.path.realpath(os.path.join(
        os.path.abspath(__file__),
        "../.."
    ))

os.environ['DJANGO_SETTINGS_MODULE'] = 'settings'

import django.core.handlers.wsgi
application = django.core.handlers.wsgi.WSGIHandler()

parent_directory = get_parent_directory()
if parent_directory not in sys.path:
    sys.path.append(parent_directory)
