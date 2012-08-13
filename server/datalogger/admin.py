from django.contrib import admin
from models import Data

class DataAdmin(admin.ModelAdmin):
    list_display = ['date', 'value']

admin.site.register(Data, DataAdmin)
