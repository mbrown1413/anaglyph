
from django.conf.urls.defaults import patterns

from anaglyph.views import calculate_view, upload_view

urlpatterns = patterns('',
    (r'^calculate/(.*)', calculate_view),
    (r'^upload/', upload_view),
    (r'^/?$', upload_view),
)
