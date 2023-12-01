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


#pragma region OTA
void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}
unsigned long ota_progress_millis = 0;
void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}
void setupOta(void) {
  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
  });
  ElegantOTA.begin(&server);
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  server.begin();
  Serial.println("HTTP server started");
}
#pragma endregion ota

void connectWifi(){
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
}

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

void utlityLoop() {
  server.handleClient();
  ElegantOTA.loop();
}

void myLoop(){
  setLED(LED_BUILTIN, millis() % 1500 < 500);
  setLED(LED_FLASH, millis() % 1500 < 1000);
}

void loop() {
  myLoop();
  utlityLoop();
}
