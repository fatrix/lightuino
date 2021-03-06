import os
import sys
sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
os.environ['DJANGO_SETTINGS_MODULE'] = 'server.settings'
import django.core.handlers.wsgi
djangoapplication = django.core.handlers.wsgi.WSGIHandler()
def application(environ, start_response):
    if 'SCRIPT_NAME' in environ:
        del environ['SCRIPT_NAME']
    return djangoapplication(environ, start_response)
