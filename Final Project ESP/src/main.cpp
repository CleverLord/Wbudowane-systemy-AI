#include <Arduino.h>

// #include <FS.h>
//define esp32cam led pins
#define LED_BUILTIN 33
#define LED_FLASH 4

// put function declarations here:
// int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_FLASH, OUTPUT);
}

void setLED(int pin, int state) {
  if (pin == LED_BUILTIN) {
    digitalWrite(LED_FLASH, state);
  } else if (pin == LED_FLASH) {
    digitalWrite(LED_BUILTIN, !state);
  }
}
  
int result = 0;
void loop() {
  // put your main code here, to run repeatedly:
  //result = myFunction(2, 3);
  Serial.println(result);
  setLED(LED_BUILTIN, millis() % 2000 < 1000);
  setLED(LED_FLASH, millis() % 2000 < 1500);
  delay(1000);
}

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }