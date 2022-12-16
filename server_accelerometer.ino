//  Libraries for Wifi & UDP:
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUDP.h>
#include "WiFiConfig.h";
#include <Wire.h>

#define DEVICE (0x53)      //ADXL345 device address
#define TO_READ (6)        //num of bytes to read each time (two bytes for each axis)

// AP parameters
IPAddress ip(192,168,8,100);
WiFiUDP Udp;

// UDP
unsigned int DestinationPort = 52791;
char packetBuffer[30]; //buffer to hold ougoing packet

// Variables ADXL345
byte buff[TO_READ] ;        //6 bytes buffer for saving data read from the device
char str[512];              //string buffer to transform data before sending it to the serial port
int regAddress = 0x32;      //first axis-acceleration-data register on the ADXL345
int x, y, z;

void setup() {
  WiFi.begin(SSID,WiFiPwd);	//get values from WiFiConfig.h
  Serial.begin(115200);
  ADXL345Setup();

}

void loop() {
    int sensorReading = analogRead(A0);
    ADXL345Loop();
    SendPacket(sensorReading);
    delay(1000);
}

void SendPacket(int sendValue) {
    sprintf(packetBuffer,"%d", sendValue);
    Udp.beginPacket(ip, DestinationPort);
    Udp.print(packetBuffer);
    Udp.endPacket();
    memset(packetBuffer,0,sizeof(packetBuffer));
}

void ADXL345Setup() {
   Wire.begin();
  //Turning on the ADXL345
  writeTo(DEVICE, 0x2D, 0);
  writeTo(DEVICE, 0x2D, 16);
  writeTo(DEVICE, 0x2D, 8);
}
void ADXL345Loop() {
  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345
                                              //each axis reading comes in 10 bit resolution, ie 2 bytes, LSB
  x = (((int)buff[1]) << 8) | buff[0];
  y = (((int)buff[3])<< 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];

  //send the x y z values as a string to the serial port
  Serial.print("The acceleration info of x, y, z are:");
  sprintf(str, "%d %d %d", x, y, z);
  Serial.print(str);
  Serial.write(10);
  delay(50);	//It appears that delay is needed in order not to clog the port
}

void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        // send register address
  Wire.write(val);        // send value to write
  Wire.endTransmission(); //end transmission
}

//reads num bytes starting from address register on device in to buff array
void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device

  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  {
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}