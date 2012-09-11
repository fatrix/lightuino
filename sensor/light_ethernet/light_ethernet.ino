/*
  Web client
 
 Inspired by:
 http://arduino.cc/en/Tutorial/WebClientRepeating
 
 NTP from:
 http://arduino.cc/en/Tutorial/UdpNtpClient
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SD.h>

// light sensor
int photocellPin = 0;
int photocellReading;

typedef enum {
  READ,
  NOTREAD
} 
STATE;


byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192,168,3,95);
//char serverName[] = "lightuino-s5zrh4wu.dotcloud.com";
char serverName[] = "192.168.3.95";
//IPAddress ip(192,168,3,199);
//IPAddress myDns(8,8,8,8);
//IPAddress gw(192,168,3,1);

// Client
EthernetClient client;

// Init url
String url;

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
boolean lastLight = false;
boolean firstRun = true;
const unsigned long log_interval = 60L*100L;
const unsigned long sync_interval = 60L*500L;

// SD
const int chipSelect = 4;
char* FILENAME = "DATALOG.TXT";


// NTP
unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress timeServer(192,43,244,18); // time.nist.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
boolean hasTime = false;

void setup() {
  // start serial port:
  Serial.begin(9600);
  // give the ethernet module time to boot up
  delay(1000);
  Ethernet.begin(mac);

  // Setup sd card -----------------------
  //Serial.print("Initializing SD card...");
  pinMode(4, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }

  Udp.begin(localPort);

}

void loop() {
  byte state = NOTREAD;
  byte c;
  int content_bytes_received = 0;
  char line_buff[100];
  int line_cursor = 0;
  // ---- handle ethernet stuff

  // Read incoming data if any available
  while (client.available()) {
    char c = client.read();
    //Serial.print(c);
    content_bytes_received++;
    line_buff[line_cursor++] = c;
    state=READ;
  }
  if (state==READ) {
    Serial.println(line_buff);
    String s = line_buff;
    if (s.startsWith("HTTP/1.0 200 OK")) {
        Serial.println("Data sent, removing file...");
        SD.remove(FILENAME);
    } else {
        Serial.println("Could not send data, remains on disk...");
    }
    Serial.flush();
  }

  // If not connected but there was one in the last loop, disconnect
  if (!client.connected() && lastConnected) {
    //Serial.println();
    //Serial.println("disconnecting.");
    client.stop();
  }

  // If not connected and it is time to connect, do the work
  if(!client.connected() && (millis() - lastConnectionTime > sync_interval)) {
    httpRequest();
  } 

  // do work in following conditions
  // - firstRun (on startup)
  // - when it is time
  // - but there is no http traffic 
  else if (firstRun || (millis() - lastLight > log_interval) && !lastConnected) {
    // log light
    char buf[50];
    int light = readLight();
    unsigned long time = gettime();

    sprintf(buf, "%d;%lu|", light, time);
    log(buf);
    lastLight = millis();

    firstRun = false;
    delay(10000);

  }

  // Store the state for the next run threw the loop
  lastConnected = client.connected();

}

// Open TCP-Connection and send a HTTP Request
void httpRequest() {
  if (client.connect(serverName, 8000)) {

    url = "POST /api/batch/" + String(" HTTP/1.1");
    File myFile = SD.open(FILENAME);
    if (myFile) {
      client.print(url+"\n");
      client.print(String("Host: ")+serverName+"\n");
      client.print("User-Agent: arduino-ethernet\n");
      client.print("Content-Length: ");
      client.println(myFile.size());
      client.println("Connection: close");
      client.println();

      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        int file_data = myFile.read();
        client.write(file_data);
      }
      // close the file:
      myFile.close();
      client.println();
      client.flush();

    } 
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening file");
      if (lastConnected) {
	 Serial.println("True");
      } else {
	 Serial.println("False");
      }

    }

    // Save the time
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    //Serial.println("connection failed");
    //Serial.println("disconnecting.");
    client.stop();
  }
}

int readLight() {
  // read light sensor
  photocellReading = analogRead(photocellPin);
  return photocellReading;
}

boolean log(String dataString) {
  File dataFile = SD.open(FILENAME, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println("Written to log: "+dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }
  //Serial.flush();
}

long gettime() {
  sendNTPpacket(timeServer); 
  delay(1000);  
  if ( Udp.parsePacket() ) {  
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  
    unsigned long secsSince1900 = highWord << 16 | lowWord;                
    const unsigned long seventyYears = 2208988800UL;     
    unsigned long epoch = secsSince1900 - seventyYears;  
    hasTime = true;
    return epoch;

  }
}

// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
}









