
//soft
void OtaErrorHandlerSetup() {
  CreateAP();
  isInSafeMode = true;
}

void OtaErrorHandlerLoop() {
  RedLedOn();
  delay(200);
  RedLedOff();
  delay(1000);
  LateInit();//To have a way to get out of the safeMode
}
//hard
