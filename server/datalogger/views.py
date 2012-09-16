# Create your views here.

from django.http import HttpResponse
from django.template import RequestContext
from django.shortcuts import render_to_response
import datetime
from models import Data, TimedData
from django.views.decorators.csrf import csrf_exempt                                        
import datetime
import pytz
from django.db import transaction


def index(request):
    #data=Data.objects.raw('SELECT id, date, value, ROUND(UNIX_TIMESTAMP(date)/(15*60)) AS timekey FROM datalogger_data GROUP BY timekey')
    data=Data.objects.raw('SELECT id, date, value, ROUND(UNIX_TIMESTAMP(date)/(15*60)) AS timekey FROM datalogger_timeddata GROUP BY timekey')
    return render_to_response('index.html', {'data': data}, context_instance=RequestContext(request))

def api(request):
    d = Data(value=request.GET.get('light'))
    print d.value
    d.save()
    return HttpResponse()

@csrf_exempt   
@transaction.commit_manually
def api_batch(request, rollback=False):
    datas = request.body.split("|")
    for d in datas:
        try:
           if d=="":
              continue
           light, time = d.split(";")
           s_date = datetime.datetime.fromtimestamp(int(time), pytz.utc)
           print light, s_date.strftime('%Y-%m-%d %H:%M:%S %Z')
            
           d = TimedData(value=light, date=s_date)
           d.save()
        except ValueError, e:
           print "ValueError"
	   print e
        except Exception, e:
           rollback=True
           print "marking transaction for rollback"
           print e
    if rollback:
        transaction.rollback()
    else:
        transaction.commit()
    return HttpResponse()
