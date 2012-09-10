from django.db import models
import time

class Data(models.Model):
    value = models.IntegerField()
    date = models.DateTimeField(auto_now=True, auto_now_add=True)

    @property
    def ms(self):
        epoch = int(time.mktime(self.date.timetuple())*1000)
        return epoch

class TimedData(models.Model):
    value = models.IntegerField()
    date = models.DateTimeField()

    @property
    def ms(self):
        epoch = int(time.mktime(self.date.timetuple())*1000)
        return epoch

