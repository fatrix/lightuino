/*
  Web client
 
 Inspired by:
 
 http://arduino.cc/en/Tutorial/WebClientRepeating
 
 NTP from:
 http://arduino.cc/en/Tutorial/UdpNtpClient
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// light sensor
int photocellPin = 0;
int photocellReading;

byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192,168,3,95);
char serverName[] = "lightuino-s5zrh4wu.dotcloud.com";
IPAddress ip(192,168,3,199);
IPAddress myDns(8,8,8,8);
IPAddress gw(192,168,3,1);

// Client
EthernetClient client;

// Init url
String url;

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long interval = 60L*1000L;

// SD
const int chipSelect = 4;


void setup() {
  // start serial port:
  Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns, gw);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  // Setup sd card -----------------------
  Serial.print("Initializing SD card...");
  pinMode(4, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  // # -----------------------------------

}

void loop() {
  //Serial.println("loop"); 


  // Read incoming data if any available
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // If not connected but there was one in the last loop, disconnect
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // If not connected and it is time to connect, do the work
  if(!client.connected() && (millis() - lastConnectionTime > interval)) {
    httpRequest();
  } 

  // Store the state for the next run threw the loop
  lastConnected = client.connected();
}

// Open TCP-Connection and send a HTTP Request
void httpRequest() {
  // if there's a successful connection:
  Serial.println("connecting...");
  if (client.connect(serverName, 80)) {

    int light = readLight();
    log(light);

    url = "GET /api/?" + String("light=") + light + String(" HTTP/1.1");

    client.println(url);
    client.println(String("Host: ")+serverName);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // Save the time
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}

int readLight() {
  // read light sensor
  photocellReading = analogRead(photocellPin);

  Serial.print("analog reading = ");
  Serial.println(photocellReading); 
  return photocellReading;
}

boolean log(int dataString) {
  File dataFile = SD.open("DATALOG.TXT", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println("Written to log: "+dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}




