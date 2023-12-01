void Log(String log) {
  //if filesystem not initialized - return
  //if time initialized, append time
  //append log to the file, add new line
  if (!spiffsInitialized) return;
  //do time here
  Serial.println(log);
  writeFile(LOGS_FILE,log+"\n",false);
}
void ClearLogs() {
  deleteFile(LOGS_FILE);
}