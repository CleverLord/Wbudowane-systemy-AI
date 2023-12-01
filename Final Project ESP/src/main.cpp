#include <Arduino.h>
#include <ArduinoOTA.h>
//Force those to be build before ota.h

#define LED_BUILTIN 33
#define LED_FLASH 4

// put function declarations here:
// int myFunction(int, int);

const char* ssid = "IoT-AP";
const char* password = "12345678";



#pragma region OTA
void setupOta(){
ArduinoOTA.setHostname("myesp32");
ArduinoOTA.setPassword("admin");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
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
  ArduinoOTA.handle();
}

void myLoop(){
  setLED(LED_BUILTIN, millis() % 1500 < 500);
  setLED(LED_FLASH, millis() % 1500 < 1000);
  Serial.println("Loop: " + String(millis()));
}

void loop() {
  myLoop();
  utlityLoop();
}
