#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "WiFiConfig.h";

// AP parameters
IPAddress ip(192,168,8,100);
IPAddress gateway(192,168,8,101);
IPAddress netmask(255,255,255,0);

unsigned int SourcePort= 0;
unsigned int LocalPort = 52791;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer for receiving data (array with dimension of packet maximum size)

WiFiUDP Udp;

void setup() {
  delay(1000);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip,gateway,netmask);
  WiFi.softAP(SSID,WiFiPwd);
  Udp.begin(LocalPort);
  Serial.begin(115200);
  
  // Print IP
  Serial.println();
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println();
    Serial.printf("Remote A0: %s",packetBuffer);
  }
  memset(packetBuffer,0,sizeof(packetBuffer));
}
