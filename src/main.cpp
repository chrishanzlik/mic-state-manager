#include <Arduino.h>

#define SWITCH 2
#define RED_LED 3
#define BLUE_LED 4

void toggleMicrophone();
void processSerialData();
bool buttonPressed();

int switchState;
int previousState = LOW;
bool mic_muted = false;
unsigned long lastToggled = 0;
unsigned long debounceTime = 500UL;

void setup() {
  Serial.begin(9600);
  pinMode(SWITCH, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}

void loop() {
  processSerialData();
  switchState = digitalRead(SWITCH);
  
  if (buttonPressed()) {
    toggleMicrophone();
    lastToggled = millis();
  }

  previousState = switchState;
}

bool buttonPressed() {
  return switchState == HIGH
    && previousState == LOW
    && millis() - lastToggled > debounceTime;
}


void toggleMicrophone() {
  int data = mic_muted ? 1 : 0;
  Serial.write(data);
  Serial.flush();
}

void processSerialData() {
  if (Serial.available() > 0) {
    while(Serial.available() > 0) {
      mic_muted = Serial.read() == 0;
    }

    digitalWrite(mic_muted ? RED_LED : BLUE_LED, HIGH);
    digitalWrite(mic_muted ? BLUE_LED : RED_LED, LOW);
  }
}