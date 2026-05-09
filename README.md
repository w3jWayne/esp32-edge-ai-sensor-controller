# ESP32 Event-Driven IoT Controller

An ESP32 (**ESP-IDF**) firmware project demonstrating a modular, event-driven IoT architecture with Wi-Fi networking, HTTP interfaces, MQTT communication, and embedded sensor data processing.

The project is designed to explore production-style embedded firmware structure using reusable modules, centralized event handling, and scalable communication flows.

---

# ✨ Highlights

* ⚡ Event-driven firmware architecture using FreeRTOS queues
* 📡 MQTT-based telemetry communication
* 🌐 Wi-Fi + HTTP server integration
* 🧩 Modular processing pipeline design
* 🔄 Centralized event dispatch system
* ⚙️ Configurable using ESP-IDF `menuconfig`
* 🧪 Python-based integration testing tools

---

# 🏗️ Architecture

```text
+-----------------------------------+
| ESP32 IoT Controller              |
|                                   |
| Sensor Input                      |
|   - Simulated                     |
|   - HTTP                          |
|                                   |
| app_sensor                        |
|      ↓                            |
| app_event                         |
|      ↓                            |
| app_pipeline                      |
|      ↓                            |
| app_decision                      |
|      ↓                            |
| app_mqtt                          |
|                                   |
| FreeRTOS Event / Queue Flow       |
+-----------------------------------+
                ↓
+-----------------------------------+
| MQTT Broker                       |
| Mosquitto                         |
+-----------------------------------+
                ↓
+-----------------------------------+
| Python Subscriber / Test Client   |
| tools/mqtt_subscriber.py          |
+-----------------------------------+
```

### Architecture Notes

* `app_event` centralizes asynchronous event handling
* `app_pipeline` manages sensor processing flow
* `app_mqtt` handles communication transport
* modules are designed to remain loosely coupled

---

# 📡 MQTT Example

## Topic

```text
esp32/iot-controller/decision
```

## Example Payload

```json
{
  "decision": "normal",
  "sensor_state": "stable",
  "sample_index": 42,
  "total_windows": 27
}
```

---

# ⚙️ Quick Start

## 1. Start MQTT Broker

```bash
mosquitto -c C:\mosquitto.conf -v
```

---

## 2. Run Python Subscriber

```bash
pip install paho-mqtt
python tools/mqtt_subscriber.py --broker <your-ip>
```

---

## 3. Configure & Flash ESP32

```bash
idf.py menuconfig
idf.py build flash monitor
```

Configure:

```text
MQTT Broker URI → mqtt://<your-ip>:1883
```

---

# 📊 Example Output

```text
[esp32/iot-controller/decision]
{
  "decision": "normal"
}

[esp32/iot-controller/decision]
{
  "decision": "monitor"
}
```

---

# 🧠 Design Highlights

## Event-Driven Architecture

* centralized event queue and dispatcher
* asynchronous module communication
* FreeRTOS queue-based task coordination
* separation between communication, processing, and control layers

---

## Modular Firmware Design

* `app_sensor` handles sensor input
* `app_pipeline` manages processing flow
* `app_decision` evaluates system state
* `app_mqtt` handles telemetry transport

This structure improves maintainability and scalability for future extensions.

---

## Correct Network Lifecycle

MQTT starts only after:

```text
Wi-Fi connected → IP_EVENT_STA_GOT_IP → MQTT start
```

This prevents early connection failures and improves network reliability.

---

# 📘 Documentation

For detailed system design and implementation notes:

* [Architecture & Design](docs/architecture.md)

---

# 🎯 What This Project Demonstrates

* ESP-IDF firmware architecture
* Event-driven embedded design
* FreeRTOS queue/task communication
* Embedded networking (Wi-Fi / HTTP / MQTT)
* Modular firmware organization
* Integration testing using Python tools
* Production-style IoT communication flow

---

# 🚀 Future Improvements

* MQTT QoS / retry handling
* Offline buffering and recovery
* Additional event sources (UART / BLE / sensors)
* Extended system monitoring and diagnostics
* OTA update support

---

# 👨‍💻 Author

Wayne Chung
Embedded Linux / Firmware Engineer
