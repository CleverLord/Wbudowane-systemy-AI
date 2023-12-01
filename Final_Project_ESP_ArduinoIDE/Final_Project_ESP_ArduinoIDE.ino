#include <Arduino.h>
//Force those to be build before ota.h
#include <FS.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>

#define LED_BUILTIN 33
#define LED_FLASH 4

// put function declarations here:
// int myFunction(int, int);

const char* ssid = "IoT-AP";
const char* password = "12345678";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_FLASH, OUTPUT);
  connectWifi();
  setupOta();
}

void setLED(int pin, int state) {
  if (pin == LED_BUILTIN) {
    digitalWrite(LED_FLASH, state);
  } else if (pin == LED_FLASH) {
    digitalWrite(LED_BUILTIN, !state);
  }
}

void myLoop(){
  setLED(LED_BUILTIN, millis() % 1000 < 500);
  setLED(LED_FLASH, millis() % 1000 < 900);
}

void loop() {
  myLoop();
  otaLoop();
}
