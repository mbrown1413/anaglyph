
from subprocess import Popen, PIPE
import os.path
from hashlib import md5

from django import forms
from django.shortcuts import render_to_response
from django.conf import settings
from django.template import RequestContext
from django.http import HttpResponse, HttpResponseBadRequest

class AnaglyphCalculateForm(forms.Form):
    choices = ((method, method) for method in settings.METHODS.keys())
    method = forms.TypedChoiceField(choices=choices,
                                    empty_value=settings.DEFAULT_METHOD)
    left_eye = forms.FileField()
    right_eye = forms.FileField()

    def clean_method(self):
        selected_method = self.cleaned_data['method']
        valid_methods = settings.METHODS.keys()
        if selected_method not in valid_methods:
            raise forms.ValidationError("Not a valid anaglyph method!")
        return selected_method

def get_image_filename(file_object):
    '''Gets the hashed filename of an uploaded image from the file object.'''
    hash = md5()
    for chunk in file_object.chunks():
        hash.update(chunk)
    filename = hash.hexdigest()
    return os.path.join(settings.IMAGE_DIRECTORY, filename)

def get_stereo_filename(left_eye_filename, right_eye_filename):
    '''Gets the hashed name of the stereo image from the input images.'''
    filename = os.path.basename(left_eye_filename) + '-' + \
               os.path.basename(right_eye_filename) + '.png'
    return os.path.join(settings.STEREO_DIRECTORY, filename)

def save_file(file_object):
    '''Saves an uploaded file to the temp directory.'''
    filename = get_image_filename(file_object)

    # Check if file is cached
    if not os.path.exists(filename):

        # Save file
        destination = open(filename, "wb+")
        for chunk in file_object.chunks():
            destination.write(chunk)
        destination.close()

    return filename

def calculate_anaglyph(executable, left_eye_filename, right_eye_filename):
    '''Calculates the anaglyph and returns an open file of the result.'''

    stereo_filename = get_stereo_filename(left_eye_filename, right_eye_filename)
    command = [
        executable,
        left_eye_filename,
        right_eye_filename,
        "-o", stereo_filename,
    ]
    print ' '.join(command)
    p = Popen(command, stdout=PIPE)
    p.wait()
    return open(stereo_filename)

def calculate_view(request):
    if request.method == 'POST':

        form = AnaglyphCalculateForm(request.POST, request.FILES)
        if form.is_valid():

            left_eye_filename = save_file(request.FILES['left_eye'])
            right_eye_filename = save_file(request.FILES['right_eye'])
            stereo_file = calculate_anaglyph(
                settings.METHODS[request.POST['method']],
                left_eye_filename,
                right_eye_filename,
            )
            return HttpResponse(stereo_file, mimetype="image/png")

    else: # First time at page
        form = AnaglyphCalculateForm()

    return render_to_response('calculate_form.html',
        {
            'form': form,
        },
        context_instance = RequestContext(request)
    )
