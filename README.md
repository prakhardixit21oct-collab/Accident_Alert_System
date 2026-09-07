# 🚨 Accident Alert System — Crash Detection with GPS Location

An Arduino-based automatic accident detection system that uses an MPU6050 accelerometer to detect crash-level impacts in real time and instantly pairs the event with a GPS location, so help can be located as fast as possible after a collision — without needing the driver to trigger anything manually.

---

## 🚗 Why this exists

In a real accident, the driver may be unable to call for help. This system removes that dependency entirely: it continuously watches for a crash-level impact and, the moment one occurs, captures the vehicle's exact GPS coordinates so the location is available immediately, automatically.

---

## ⚙️ How it works

**1. Continuous crash monitoring**
Every 100ms, the system reads acceleration data (X, Y, Z) from an **MPU6050** accelerometer over I2C and computes the total G-force experienced by the vehicle:

```
totalG = √(Ax² + Ay² + Az²)
```

**2. Crash detection**
If the combined G-force exceeds **2.0G** — well beyond normal driving forces (braking, cornering, potholes) — the system flags it as a crash and immediately stops further checks, so a single confirmed event isn't overwritten.

**3. GPS location capture**
On trigger, the system reads live coordinates from a **GPS module** (via `TinyGPS++` over software serial) and prints both the raw latitude/longitude and a ready-to-use Google Maps link, so a location is human-readable instantly:
```
https://maps.google.com/?q=<lat>,<lng>
```
If a GPS fix isn't available yet at the moment of the crash, the system reports that clearly instead of sending an empty/invalid location.

**4. I2C slave interface**
This module runs as an **I2C slave device**, exposing the `accidentTriggered` flag on request (`Wire.onRequest`). This is intentional — it's designed to be polled by a separate master controller (e.g. a GSM/communication module or the vehicle's main dashboard controller), which can then handle actually sending an SMS/alert, keeping crash-sensing and communication logic cleanly separated.

---

## 🔩 Hardware Used

| Component | Purpose |
|---|---|
| Arduino (I2C slave) | Main controller for crash detection |
| MPU6050 | 3-axis accelerometer for impact detection |
| GPS Module (UART) | Location capture on crash |
| I2C bus | Reports crash status to a master controller |

---

## 🛡️ Detection Logic

| Condition | Result |
|---|---|
| Total G-force > 2.0G | Crash flagged, alert triggered |
| GPS fix available at trigger | Latitude/longitude + Maps link logged |
| GPS fix not available | Logged as "GPS FIX NOT AVAILABLE YET" |
| After trigger | System stops re-checking to avoid duplicate/false re-triggers |

---

## 🧰 Tech Stack

- **Platform:** Arduino (I2C slave device)
- **Libraries:** `Wire.h`, `SoftwareSerial.h`, `TinyGPS++`
- **Language:** C++ (Arduino)

---

## 🚀 Getting Started

1. Wire the MPU6050 to the I2C bus and the GPS module to the software serial pins (RX=4, TX=3) as defined in `Accident_Alert.ino`.
2. Open the sketch in the Arduino IDE with the `TinyGPS++` library installed.
3. Flash it to the Arduino, set as I2C slave address `0x09`.
4. Connect a master controller (e.g. GSM module) on the same I2C bus to poll `accidentTriggered` and handle alert dispatch.

---

## 🔭 Future Improvements

- Add a debounce/confirmation window (e.g. require elevated G for 2+ consecutive reads) to reduce false positives from potholes
- Directly integrate a GSM module to auto-send SMS alerts instead of relying solely on I2C polling
- Log crash events with timestamp for post-incident analysis
- Add a manual reset button instead of requiring a power cycle after a trigger

---

## 👤 Author

**Prakhar Dixit**
Electronics & Communication Engineering (VLSI Design & Technology) — Zakir Husain College of Engineering & Technology, AMU
Recruitment & Workshop Coordinator and contributor, SAE ZHCET
[LinkedIn](https://www.linkedin.com/in/prakhar-dixit-a2bb95377)
