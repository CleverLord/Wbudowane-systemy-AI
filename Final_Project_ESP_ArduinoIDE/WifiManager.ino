//public:
void ConnectWiFi(String ssid, String pass) {
  WiFi.mode(WIFI_STA);  //station (Wifi Station = client)
  WiFi.disconnect();
  Serial.println("Connecting to:");
  Serial.println(ssid);
  Serial.println(pass);
  WiFi.begin(ssid.c_str(), pass.c_str());
}

//private:
IPAddress local_IP(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

//public:
void CreateAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPdisconnect(true);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(GenerateSSID().c_str(), GeneratePassword().c_str());
}

//private:
String GenerateSSID() {
  String mac = String(WiFi.macAddress());
  mac.replace(":", "");
  return "IoT_ISI " + mac.substring(4, 7) + "-" + mac.substring(7, 12);  // Max ssid name is 32 chars, ending is 9 (7 chars + '-' + ' ') + 23 chars for name
}

String GeneratePassword() {
  String mac = String(WiFi.macAddress());
  mac.replace(":", "");
  return mac.substring(4, 12).c_str();
}
