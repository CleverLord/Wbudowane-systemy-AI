void Init() {
  InitGpio();
  if (!InitSpiffs()) return;
  Log("Device Powered, Spiffs working");  //This is the first place we can use this function
  InitWifi();
  Log(GetWifiMode());
  Log(GetWifiStatus());
  InitOtaAndHttp();
  if (!ProcessBootAttempts()) return;

  //PrewarmFpsProcessor(); //todo - measure lags
  esp_task_wdt_init(7, true);  //włączenie obsługi watchdog ESP32
  esp_task_wdt_add(NULL);                //dodanie bieżącego wątku do "pilnowania" przez WDT

  initSuccess = true;
  Log("Device running!");
}

void LateInit() {
  if (bootFileDeleted) return;
  if (millis() > 10000) {
    deleteFile(BOOT_FILE);  //delete file after ~10 seconds of uninterrupted working state
    bootFileDeleted = true;
    Log("DeletedBootFile");
  }
}

//private:
void InitGpio() {
  pinMode(RED_LED, OUTPUT);
  pinMode(FLASH_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);  //Low=On
  digitalWrite(FLASH_LED, LOW);
}

void InitOtaAndHttp() {
  SetupSafeHttpEndpoints();
  AsyncElegantOTA.begin(&server);  //Start ElegantOTA
  server.begin();  //Second error here                //this only launches socket for connections. Further functions can be added
  Log("server.begin(); ");
}

void InitWifi() {
  if (WiFi.getMode() != 0) {
    Log(F("Wifi already set, skipping stage"));
    return;
  }
  if (fileExists(WIFI_SSID_FILE) && fileExists(WIFI_PASS_FILE)) {
    Log(F("credientials detected, connecting"));
    ConnectUsingCredientialsInFiles();
  } else {
    //ConnectWiFi("Orange_Swiatlowod_C530", "A6SDDR2S2959");
    Log("Creating AP");
    CreateAP();
  }
}
void ConnectUsingCredientialsInFiles() {
  ConnectWiFi(getFile(WIFI_SSID_FILE), getFile(WIFI_PASS_FILE));
}

bool ProcessBootAttempts() {
  writeFile(BOOT_FILE, ".", false);            // add dot to the file
  bootAttempts = getFile(BOOT_FILE).length();  // get number of dots in file
  bool bootAttemptsVerdict = (bootAttempts < 3);
  Log("Boot attempts count: " + String(bootAttempts) + ", returning " + String(bootAttemptsVerdict));
  return bootAttemptsVerdict;
}