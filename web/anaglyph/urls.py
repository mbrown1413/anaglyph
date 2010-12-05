
from django.conf.urls.defaults import patterns

from anaglyph.views import calculate_view

urlpatterns = patterns('',
    (r'^calculate/', calculate_view),
)
