/*
 * Smart Dustbin - Embedded IoT & Automation Firmware
 * Compatible with ESP32 / Arduino boards
 *
 * Features:
 *  - Touchless lid opening using front HC-SR04 ultrasonic sensor + Servo motor
 *  - Internal fill-level measurement using top-mounted HC-SR04 ultrasonic sensor
 *  - Overflow alert LED when bin fill >= 85%
 *  - JSON telemetry output over Serial (115200 baud) for IoT dashboard integration
 */

#include <Arduino.h>
#if defined(ESP32)
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

// --- Pin Definitions ---
const int TRIG_LID_PIN   = 5;   // Front sensor Trig (hand detection)
const int ECHO_LID_PIN   = 18;  // Front sensor Echo
const int TRIG_LVL_PIN   = 19;  // Internal sensor Trig (fill level)
const int ECHO_LVL_PIN   = 21;  // Internal sensor Echo
const int SERVO_PIN      = 13;  // Lid servo signal pin
const int ALERT_LED_PIN  = 2;   // Overflow indicator LED

// --- Configuration Constants ---
const float BIN_HEIGHT_CM       = 40.0;  // Total internal height of dustbin (cm)
const float HAND_THRESHOLD_CM   = 25.0;  // Trigger distance for touchless lid
const int   OVERFLOW_THRESHOLD  = 85;    // Percentage threshold for overflow alert
const int   LID_CLOSED_ANGLE    = 0;
const int   LID_OPEN_ANGLE      = 90;
const unsigned long LID_HOLD_MS = 3000;  // Keep lid open for 3 seconds
const unsigned long TELEMETRY_INTERVAL_MS = 2000;

Servo lidServo;
bool isLidOpen = false;
unsigned long lidOpenedAt = 0;
unsigned long lastTelemetryAt = 0;
unsigned long touchlessCount = 0;

float measureDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  if (duration == 0) {
    return -1.0; // Out of range / no echo
  }
  return (duration * 0.0343f) / 2.0f;
}

int calculateFillPercent(float distanceCm) {
  if (distanceCm < 0) return 0;
  float clamped = constrain(distanceCm, 0.0f, BIN_HEIGHT_CM);
  float fill = ((BIN_HEIGHT_CM - clamped) / BIN_HEIGHT_CM) * 100.0f;
  return constrain((int)round(fill), 0, 100);
}

void openLid() {
  if (!isLidOpen) {
    lidServo.write(LID_OPEN_ANGLE);
    isLidOpen = true;
    touchlessCount++;
  }
  lidOpenedAt = millis();
}

void closeLid() {
  if (isLidOpen) {
    lidServo.write(LID_CLOSED_ANGLE);
    isLidOpen = false;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_LID_PIN, OUTPUT);
  pinMode(ECHO_LID_PIN, INPUT);
  pinMode(TRIG_LVL_PIN, OUTPUT);
  pinMode(ECHO_LVL_PIN, INPUT);
  pinMode(ALERT_LED_PIN, OUTPUT);

  lidServo.attach(SERVO_PIN);
  lidServo.write(LID_CLOSED_ANGLE);
  digitalWrite(ALERT_LED_PIN, LOW);

  Serial.println("{\"status\":\"Smart Dustbin Initialized\"}");
}

void loop() {
  unsigned long now = millis();

  // 1. Check front sensor for touchless lid activation
  float handDistance = measureDistanceCm(TRIG_LID_PIN, ECHO_LID_PIN);
  if (handDistance > 0 && handDistance <= HAND_THRESHOLD_CM) {
    openLid();
  }

  // 2. Automatically close lid after hold duration
  if (isLidOpen && (now - lidOpenedAt >= LID_HOLD_MS)) {
    closeLid();
  }

  // 3. Measure internal fill level & publish JSON telemetry
  if (now - lastTelemetryAt >= TELEMETRY_INTERVAL_MS) {
    lastTelemetryAt = now;

    // Only measure fill level when lid is closed for accurate reading
    int fillPercent = 0;
    if (!isLidOpen) {
      float wasteDistance = measureDistanceCm(TRIG_LVL_PIN, ECHO_LVL_PIN);
      fillPercent = calculateFillPercent(wasteDistance);
    }

    bool overflowAlert = (fillPercent >= OVERFLOW_THRESHOLD);
    digitalWrite(ALERT_LED_PIN, overflowAlert ? HIGH : LOW);

    Serial.print("{\"lidOpen\":");
    Serial.print(isLidOpen ? "true" : "false");
    Serial.print(",\"fillPercent\":");
    Serial.print(fillPercent);
    Serial.print(",\"overflowAlert\":");
    Serial.print(overflowAlert ? "true" : "false");
    Serial.print(",\"touchlessOpenings\":");
    Serial.print(touchlessCount);
    Serial.println("}");
  }

  delay(50);
}
