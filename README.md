lightuino
=========

Lightuino - Light sensor datalogger

[Demo](http://lightuino-s5zrh4wu.dotcloud.com/)
[Screenshot from demo](screenshot-lightuino-datalogger.png "Screenshot from demo")


Server
------
Simple Datalogger API 

### Configuration
- Add DATABASES to server/local_settings.py
- cd server/ ; python manage syncdb

### Usage locally
> cd server/
> python manage runserver

Visit [devserver](http://127.0.0.1:8000)

### Usage on dotcloud
dotcloud push APPNAME server/

Sensor
------
A sketch for arduino light sensor with Arduino Ethernetshield and a light sensor (LDR)

### Configuration
- Change network settings in light_ethernet.ino
- Change API-Endpoint in light_ethernet.ino

### Installation
- Upload sensor/sensor/light_ethernet/light_ethernet.ino onto your Arduino
