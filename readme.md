# Smart Dustbin

Smart Dustbin is an innovative waste management solution that aims to make garbage disposal smarter, cleaner, and more efficient. By integrating embedded systems, sensors, and automation, the system simplifies traditional waste disposal processes.

The project focuses on improving hygiene, reducing human effort, and encouraging responsible waste management. It demonstrates how IoT and automation technologies can be applied to solve real-world environmental problems.

## Key Objectives

- Improve waste disposal and management.
- Reduce manual interaction with dustbins.
- Promote cleanliness and hygiene.
- Minimize waste overflow.
- Encourage sustainable environmental practices.

## Technologies Used

- **IoT** — Real-time telemetry and remote fill-level monitoring
- **Embedded Systems** — Microcontroller-based control loop
- **Sensors** — Ultrasonic distance sensors (HC-SR04) for touchless lid detection and internal fill-level measurement
- **Microcontrollers** — ESP32 / Arduino-compatible board
- **Automation** — Servo-actuated lid mechanism and automated overflow alerts

## Project Structure

```text
Smart_dustbin/
├── README.md                  # Project documentation
├── firmware/
│   └── smart_dustbin.ino      # ESP32 / Arduino firmware for sensors, servo lid, and serial telemetry
└── dashboard/
    └── smart_dustbin_dashboard.html  # Interactive IoT monitoring & simulation dashboard
```

## Hardware Components

| Component | Purpose |
| :--- | :--- |
| ESP32 / Arduino Uno | Main microcontroller processing sensor data and controlling actuators |
| Ultrasonic Sensor #1 (HC-SR04) | Detects user hand/object proximity for touchless lid opening |
| Ultrasonic Sensor #2 (HC-SR04) | Measures remaining internal bin space to calculate fill percentage |
| Servo Motor (SG90 / MG90S) | Opens and closes the dustbin lid automatically |
| Status LED & Buzzer (Optional) | Alerts when the bin reaches overflow threshold (>= 85%) |

## How It Works

1. **Touchless Lid Automation:** When a user approaches within `25 cm` of the front ultrasonic sensor, the microcontroller commands the servo motor to open the lid (`90°`), holds it open for `3 seconds`, and smoothly closes it (`0°`).
2. **Fill-Level Monitoring:** After the lid closes (and periodically every `2 seconds`), the internal ultrasonic sensor measures the distance to the waste surface and computes the fill percentage.
3. **Overflow Prevention:** Once the fill level reaches `85%` or higher, the system activates the alert LED and flags an overflow warning on the IoT dashboard so the bin can be emptied promptly.

## Getting Started

1. **Upload Firmware:** Open `smart_dustbin.ino` in the Arduino IDE, wire the ultrasonic sensors and servo motor according to the pin definitions at the top of the sketch, and upload it to your board.
2. **Launch Dashboard:** Open `smart_dustbin_dashboard.html` in any modern web browser to monitor bin status, inspect telemetry logs, and simulate sensor triggers.
