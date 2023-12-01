// while (millis() - start < ms) ;  // Bug free version of time measuring
#include <NTPClient.h>
#include <WiFiUdp.h>

const long utcOffsetInSeconds = 3600;
const char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const ulong deltaBetweenTimeUpdate = 30000;  //once per 30 seconds
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

ulong timeClientlastMillis = 0;

void TimeInit() {
  timeClient.begin();
  timeClientlastMillis = millis();
}
void UpdateTimeClient() {
  if (millis() - timeClientlastMillis < deltaBetweenTimeUpdate) return;
  ForceUpdateTimeClient();
  timeClientlastMillis = millis();
}
void ForceUpdateTimeClient() {
  timeClient.update();
}