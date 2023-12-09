// while (millis() - start < ms) ;  // Bug free version of time measuring
ulong timeClientlastMillis = 0;

void TimeInit() {
  timeClient.begin();
  timeClientlastMillis = millis();
}
void UpdateTimeClient() {
  timeClient.update();
}