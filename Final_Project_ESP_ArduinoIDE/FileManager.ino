#include <FS.h>
#include "SPIFFS.h"

bool InitSpiffs() {
  spiffsInitialized = SPIFFS.begin(true);
  return spiffsInitialized;
}

String getFile(String path) {
  String buf;
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    while (file.available())
      buf += char(file.read());  //optimize this later using file.size() and char* (malloc)
    file.close();
    return buf;
  } else {
    //Serial.println("\tFileNotFound");
    Log("Requested access to nonexisting file on path <" + path + ">");
    return "";
  }
}

void writeFile(String path, String data, bool clearFile = true) {
  String buf;
  File file;
  if (clearFile)
    file = SPIFFS.open(path, "w");
  else
    file = SPIFFS.open(path, "a");
  file.print(data);
  file.close();
}

void deleteFile(String path) {
  SPIFFS.remove(path);
}

bool fileExists(String path) {
  return SPIFFS.exists(path);
}