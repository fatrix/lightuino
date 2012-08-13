lightuino
=========

Lightuino - Light sensor datalogger

Sensor
------
A sketch for arduino light sensor with Arduino Ethernetshield and a light sensor (LDR)

### Installation
Install sensor/sensor/light_ethernet/light_ethernet.ino onto your Arduino

Server
------
Simple Datalogger API 

### Configuration
- Add DATABASES to server/local_settings.py
- cd server/ ; python manage syncdb

### Usage locally
> cd server/
> python manage runserver

Visit [devserver(http://127.0.0.1:8000)

### Usage on dotcloud
dotcloud push APPNAME server/

