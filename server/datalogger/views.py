# Create your views here.

from django.http import HttpResponse
from django.template import RequestContext
from django.shortcuts import render_to_response
import datetime
from models import Data
from django.views.decorators.csrf import csrf_exempt                                          


def index(request):
    data=Data.objects.raw('SELECT id, date, value, ROUND(UNIX_TIMESTAMP(date)/(15*60)) AS timekey FROM datalogger_data GROUP BY timekey')
    return render_to_response('index.html', {'data': data}, context_instance=RequestContext(request))

def api(request):
    d = Data(value=request.GET.get('light'))
    print d.value
    d.save()
    return HttpResponse()

@csrf_exempt   
def api_batch(request):
    print request.body
    #d = Data(value=request.GET.get('light'))
    #print d.value
    #d.save()
    return HttpResponse()
