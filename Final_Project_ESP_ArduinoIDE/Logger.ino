
void Log(String log) {
  //if filesystem not initialized - return
  //if time initialized, append time
  //append log to the file, add new line
  if (!spiffsInitialized) return;
  String t="["+timeClient.getFormattedTime()+"] ";
  Serial.println(t+log);
  writeFile(LOGS_FILE,t+log+"\n",false);
}
void LogInline(String log, bool isLast=false) {
  if (!spiffsInitialized) return;
  if (isLast) log+="\n";
  Serial.print(log);
  writeFile(LOGS_FILE,log,false);
}

void ClearLogs() {
  deleteFile(LOGS_FILE);
}