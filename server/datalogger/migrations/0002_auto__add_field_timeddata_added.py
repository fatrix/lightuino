# -*- coding: utf-8 -*-
import datetime
from south.db import db
from south.v2 import SchemaMigration
from django.db import models


class Migration(SchemaMigration):

    def forwards(self, orm):
        # Adding field 'TimedData.added'
        db.add_column('datalogger_timeddata', 'added',
                      self.gf('django.db.models.fields.DateTimeField')(auto_now=True, auto_now_add=True, default='1900-01-01 00:00:00', blank=True),
                      keep_default=False)


    def backwards(self, orm):
        # Deleting field 'TimedData.added'
        db.delete_column('datalogger_timeddata', 'added')


    models = {
        'datalogger.data': {
            'Meta': {'object_name': 'Data'},
            'date': ('django.db.models.fields.DateTimeField', [], {'auto_now': 'True', 'auto_now_add': 'True', 'blank': 'True'}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'value': ('django.db.models.fields.IntegerField', [], {})
        },
        'datalogger.timeddata': {
            'Meta': {'object_name': 'TimedData'},
            'added': ('django.db.models.fields.DateTimeField', [], {'auto_now': 'True', 'auto_now_add': 'True', 'blank': 'True'}),
            'date': ('django.db.models.fields.DateTimeField', [], {}),
            'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'value': ('django.db.models.fields.IntegerField', [], {})
        }
    }

    complete_apps = ['datalogger']