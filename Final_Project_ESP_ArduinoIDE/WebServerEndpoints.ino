void SetupSafeHttpEndpoints() {
  //Only fully safe endpoints here - there are included in the safe mode
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });
  server.on("/help", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "GetMac\nRestart\nDeleteBootFile\nLogs\nClearLogs\nListFiles\nSetWifiSettings?"+String(WIFI_SSID_SERVER_INPUT_NAME)+"=newName"+String(WIFI_PASS_SERVER_INPUT_NAME)+"=newPass");
  });
  server.on("/GetMac", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(WiFi.macAddress()));
  });
  server.on("/Restart", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK, restarting");
    ESP.restart();
  });
  server.on("/DeleteBootFile", HTTP_GET, [](AsyncWebServerRequest *request) {
    deleteFile(BOOT_FILE);
    request->send(200, "text/plain", "File deleted");
  });  
  server.on("/Logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!spiffsInitialized) {
      request->send(200, "text/plain", "Spiffs not initialized");
      return;
    }
    request->send(200, "text/plain", getFile(LOGS_FILE));
  });
  server.on("/ClearLogs", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!spiffsInitialized) {
      request->send(200, "text/plain", "Spiffs not initialized");
      return;
    }
    ClearLogs();
    request->send(200, "text/plain", "Ok");
  });
  server.on("/ListFiles", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!spiffsInitialized) {
      request->send(200, "text/plain", "Spiffs not initialized");
      return;
    }
    request->send(200, "text/plain", getAllFiles().c_str());
  });
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Page not found.");
  });

  server.on("/SetWifiSettings", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!spiffsInitialized) {
      request->send(200, "text/plain", "Spiffs not initialized");
      return;
    }
    String inputSsid;
    String inputPass;
    if (request->hasParam(WIFI_SSID_SERVER_INPUT_NAME) && request->hasParam(WIFI_PASS_SERVER_INPUT_NAME)) {
      inputSsid = request->getParam(WIFI_SSID_SERVER_INPUT_NAME)->value();
      inputPass = request->getParam(WIFI_PASS_SERVER_INPUT_NAME)->value();
      Serial.println("New SSID: " + inputSsid);
      Serial.println("New Password: " + inputPass);
      writeFile(WIFI_SSID_FILE, inputSsid);
      writeFile(WIFI_PASS_FILE, inputPass);
      request->send(200, "text/html", "SSID and Password set");
      return;
    }
    request->send(200, "text/html", "Missing parameters");
  });
  //server.on("/Admin", HTTP_GET, )
  //return setdevicenameform
  //server.on("/SetDeviceName", HTTP_POST, )
  //if Spiffs working, save name
}
String getAllFiles() {
  String result = "";
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    result += String(file.name());
    result += "\n";
    file = root.openNextFile();
  }
  return result;
}
