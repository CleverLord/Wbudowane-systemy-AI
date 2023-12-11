#define WIFI_SSID_FILE "/WiFi_SSID.txt"
#define WIFI_PASS_FILE "/WiFi_pass.txt"
#define WIFI_SSID_SERVER_INPUT_NAME "wifi_ssid"
#define WIFI_PASS_SERVER_INPUT_NAME "wifi_pass"
//TODO: update to file with "saved networks"
#define BOOT_FILE "/boot.txt"
#define LOGS_FILE "/logs.txt"
#define DEVICE_NAME_FILE "/deviceName.txt"
#define BUILD_TIME String(BUILD_DAY)+"."+String(BUILD_MONTH)+"."+String(BUILD_YEAR)+" "+String(BUILD_HOUR)+":"+String(BUILD_MIN)+":"+String(BUILD_SEC)

bool initSuccess = false;

AsyncWebServer server(80);
bool spiffsInitialized = false;
bool timeInicialized = false;
bool bootFileDeleted = false;

const long utcOffsetInSeconds = 3600;
const char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const ulong deltaBetweenTimeUpdate = 30000;  //once per 30 seconds
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int bootAttempts = -1;
int lastFrameFPS = 0;

//GlobalVars
double a=0.25;
double T=800;
