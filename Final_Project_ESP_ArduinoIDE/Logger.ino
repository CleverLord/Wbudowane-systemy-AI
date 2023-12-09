void Log(String log) {
  //if filesystem not initialized - return
  //if time initialized, append time
  //append log to the file, add new line
  if (!spiffsInitialized) return;
  //do time here
  Serial.println(log);
  writeFile(LOGS_FILE,log+"\n",false);
}
void LogInline(String log) {
  //if filesystem not initialized - return
  //if time initialized, append time
  //append log to the file, add new line
  if (!spiffsInitialized) return;
  //do time here
  Serial.print(log);
  writeFile(LOGS_FILE,log,false);
}
void ClearLogs() {
  deleteFile(LOGS_FILE);
}