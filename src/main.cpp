#include <Arduino.h>

#define SWITCH 2
#define RED_LED 3
#define BLUE_LED 4

enum MicState {
  Unknown = 0,
  Muted = 1,
  Activated = 2
};

void toggleMicrophone();
void processSerialData();
bool buttonPressed();
void checkIdle();

int switch_state;
int previous_switch_state = LOW;
MicState mic_state = MicState::Unknown;
unsigned long last_toggled_ms = 0;
unsigned long debounce_time_ms = 500UL;
unsigned long last_received_ms = 0;

void setup() {
  pinMode(SWITCH, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  _delay_ms(500);

  Serial.begin(9600);

  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
}

void loop() {
  processSerialData();
  switch_state = digitalRead(SWITCH);
  
  if (buttonPressed()) {
    toggleMicrophone();
    last_toggled_ms = millis();
  }

  previous_switch_state = switch_state;

  checkIdle();
}

bool buttonPressed() {
  return switch_state == HIGH
    && previous_switch_state == LOW
    && millis() - last_toggled_ms > debounce_time_ms;
}

void checkIdle() {
  bool isIdle = last_received_ms == 0 || millis() - last_received_ms > 1000 * 10;

  if (isIdle) {
    _delay_ms(200);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
    _delay_ms(1500);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
  }
}

void toggleMicrophone() {
  int data = mic_state == MicState::Activated ? 1 : 0;
  Serial.write(data);
  Serial.flush();
}

void processSerialData() {
  if (Serial.available() > 0) {

    last_received_ms = millis();

    while(Serial.available() > 0) {
      int data = Serial.read();

      if (data == 6)
      {
        _delay_ms(100);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(BLUE_LED, HIGH);
        Serial.write(6);
        Serial.flush();
        return;
      }

      mic_state = data == 0 ? MicState::Muted : MicState::Activated;
    }

    digitalWrite(mic_state == MicState::Muted ? RED_LED : BLUE_LED, HIGH);
    digitalWrite(mic_state == MicState::Muted ? BLUE_LED : RED_LED, LOW);
  }
}