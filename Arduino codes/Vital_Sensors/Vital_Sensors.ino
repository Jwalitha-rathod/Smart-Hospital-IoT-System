#include <PulseSensorPlayground.h>

const int PulseWire = A0;
const int ECG_PIN = A1;
const int LO_PLUS = 10;
const int LO_MINUS = 11;

PulseSensorPlayground pulseSensor;

bool measuring = false;

void setup() {

  Serial.begin(115200);
  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(550);

  pulseSensor.begin();

  Serial.println("READY");
}

void loop() {

  if (Serial.available()) {

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "PULSE") {

      measuring = true;

      Serial.println("WAITING_FOR_FINGER");
    }
  }

  if (measuring) {

  static bool fingerDetected = false;
  static unsigned long startTime = 0;
  static int sum = 0;
  static int count = 0;

  if (pulseSensor.sawStartOfBeat()) {

    int bpm = pulseSensor.getBeatsPerMinute();

    if (!fingerDetected) {
      Serial.println("FINGER_DETECTED");
      Serial.println("MEASURING...");
      fingerDetected = true;
      startTime = millis();
    }

    if (bpm > 40 && bpm < 180) {
      sum += bpm;
      count++;
    }

    if (millis() - startTime >= 5000) {

  if (count > 0) {

    int avg = sum / count;

    Serial.print("PULSE:");
    Serial.println(avg);

  } else {

    Serial.println("NO_PULSE_DETECTED");

  }

  // =========================
  // ECG Measurement
  // =========================

  if (digitalRead(LO_PLUS) == HIGH || digitalRead(LO_MINUS) == HIGH) {

    Serial.println("ECG:-1");

  } else {

    long ecgSum = 0;

    for (int i = 0; i < 200; i++) {

      ecgSum += analogRead(ECG_PIN);
      delay(2);

    }

    int avgECG = ecgSum / 200;

    Serial.print("ECG:");
    Serial.println(avgECG);

  }

  measuring = false;
  fingerDetected = false;
  sum = 0;
  count = 0;

  Serial.println("READY");
}
  }
}
}