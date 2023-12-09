void CoreSetup() {
  Init();
  if (!initSuccess)
    OtaErrorHandlerSetup();
  else
    AppSetup();
}

void CoreLoop() {
  esp_task_wdt_reset();

  if (!spiffsInitialized) {
    //Very Bad - Esp internal issue
    RedLedOn();
    delay(750);
    RedLedOff();
    delay(2000);
    return;
  }

  if (!initSuccess) {
    //Casual bad - Devs' code problem
    //Safe mode ( Access Point + Ota enabled only )
    OtaErrorHandlerLoop();
    return;
  }

  //regular code
  LateInit();
  AppLoop();
  ProcessFps();
}
