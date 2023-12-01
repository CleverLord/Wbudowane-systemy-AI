void AppSetup() {
  //Serial already configured to 115200 (not true for now)
  //Leds (GPIO 4 and 33) already set to OUTPUT
  //SPIFFS already running
  //WebServer is already up, but you can still add endpoints

  // configure LED PWM functionalitites
  ledcSetup(0, 5000, 8);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(RED_LED, 0);
  server.on("/a", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("a")) {
      String aStr = request->getParam("a")->value();
      Log("New a: " + aStr);
      a = aStr.toDouble();
      request->send(200, "text/html", ("a set to " + String(a)).c_str());
      return;
    }
    request->send(200, "text/html", "Missing parameters");
  });
  server.on("/T", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("T")) {
      String tStr = request->getParam("T")->value();
      Log("New T: " + tStr);
      T = tStr.toDouble();
      request->send(200, "text/html", ("T set to " + String(T)).c_str());
      return;
    }
    request->send(200, "text/html", "Missing parameters");
  });
}

void AppLoop() {
  //This is your sample code
  
  //Dummy heartbeat below
  double t = millis() / T;
  double tans = abs(tan(t)) * abs(tan(t + a));
  ledcWrite(0, 255 - min((int)tans, 255));
  //ledcWrite(0, );
  //ledcWrite(0, 255-abs(sin(millis()/1000.0)*255) );
  delay(10);
}
