#define WIFI_SSID_FILE "/WiFi_SSID.txt"
#define WIFI_PASS_FILE "/WiFi_pass.txt"
#define WIFI_SSID_SERVER_INPUT_NAME "wifi_ssid"
#define WIFI_PASS_SERVER_INPUT_NAME "wifi_pass"
//TODO: update to file with "saved networks"
#define BOOT_FILE "/boot.txt"
#define LOGS_FILE "/logs.txt"
#define DEVICE_NAME_FILE "/deviceName.txt"

bool initSuccess = false;

AsyncWebServer server(80);
bool spiffsInitialized = false;
bool timeInicialized = false;
bool bootFileDeleted = false;

int bootAttempts = -1;
int lastFrameFPS = 0;

//GlobalVars
double a=0.25;
double T=800;
