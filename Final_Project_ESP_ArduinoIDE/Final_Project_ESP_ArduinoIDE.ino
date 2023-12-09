#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <esp_task_wdt.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <buildTime.h>

#define WDT_TIMEOUT 20

#define RED_LED 33
#define FLASH_LED 4

//Functions Declarations
void Init();
void LateInit();

String GetWifiMode();
String GetWifiStatus();

// WifiManager;
void ConnectWiFi(String ssid, String pass);
void CreateAP();

void setup(void) {
  Serial.begin(115200); //TEMP
  CoreSetup();
}
void loop() {
  CoreLoop();
}
