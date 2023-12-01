void SafeReset(){ESP.restart();}

void RedLedOn(){    digitalWrite(RED_LED,LOW);    }
void RedLedOff(){   digitalWrite(RED_LED,HIGH);   }
void FlashLedOff(){ digitalWrite(FLASH_LED,LOW);  }
void RedLedAsync(bool isOn){    digitalWrite(RED_LED,!isOn);}
void FlashLedAsync(bool isOn){  digitalWrite(FLASH_LED,isOn);}

String ifString(bool condition, String ifYes, String ifNo){
  return (condition ? ifYes : ifNo);
}

//TODO: make use of below
ulong frames = 0;
ulong lastSec = 0;
void ProcessFps() {
  frames++;
  uint sec = millis() / 1000;
  if (sec != lastSec) {
    lastSec = sec;
    if (frames < 100) {
      Log("Throttle warning: fps = " + String(frames));
    }
    lastFrameFPS = frames;
    frames = 0;
  }
}
//EndofTODO

String GetWifiMode() {
  const String stateStr[] = { "WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA" };
  return stateStr[WiFi.getMode()];
}
String GetWifiStatus() {
  const String stateStr[] = { "WL_IDLE_STATUS", "WL_NO_SSID_AVAIL", "WL_SCAN_COMPLETED", "WL_CONNECTED", "WL_CONNECT_FAILED", "WL_CONNECTION_LOST", "WL_DISCONNECTED" };
  int x = WiFi.status();
  if (x == 255)
    return "WL_NO_SHIELD";
  if (x >= 8)
    return "Weird status";
  return stateStr[WiFi.status()];
}

