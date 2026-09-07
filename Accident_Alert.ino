#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#define ADDR 0x09

/* ================= GPS ================= */
SoftwareSerial gpsSerial(4, 3);   // Arduino RX, TX
TinyGPSPlus gps;

/* ================= MPU6050 ================= */
#define MPU 0x68

/* ================= STATE ================= */
bool accidentTriggered = false;
unsigned long lastMPURead = 0;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Wire.begin(ADDR);        // SLAVE MODE
  Wire.onRequest(sendData);

  /* Initialize MPU6050 */
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);    // Power management register
  Wire.write(0);       // Wake up MPU6050
  Wire.endTransmission(true);

  Serial.println("ACCIDENT ALERT SYSTEM READY (AUTO MODE)");
}

void loop() {

  /* Always read GPS (non-blocking) */
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  /* Read MPU every 100 ms */
  if (!accidentTriggered && millis() - lastMPURead >= 100) {
    lastMPURead = millis();
    checkCrash();
  }
}

/* ================= CRASH DETECTION ================= */
void checkCrash() {
  int16_t ax, ay, az;

  Wire.beginTransmission(MPU);
  Wire.write(0x3B);              // Accelerometer data start
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();

  float Ax = ax / 16384.0;
  float Ay = ay / 16384.0;
  float Az = az / 16384.0;

  float totalG = sqrt(Ax * Ax + Ay * Ay + Az * Az);

  Serial.print("Total G: ");
  Serial.println(totalG);

  /* Crash threshold */
  if (totalG > 2.0) {
    Serial.println("AUTO CRASH DETECTED");
    triggerAccident();
  }
}

/* ================= ALERT HANDLER ================= */
void triggerAccident() {
  accidentTriggered = true;

  Serial.println("🚨 ACCIDENT ALERT 🚨");

  if (gps.location.isValid()) {
    Serial.print("Latitude  : ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude : ");
    Serial.println(gps.location.lng(), 6);

    Serial.print("Google Maps: ");
    Serial.print("https://maps.google.com/?q=");
    Serial.print(gps.location.lat(), 6);
    Serial.print(",");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("GPS FIX NOT AVAILABLE YET");
  }
}
void sendData() {
  Wire.write(accidentTriggered);
}