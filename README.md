# ESP32 Edge AI IoT Controller

An ESP32 (ESP-IDF) project that performs **real-time anomaly detection on-device** and publishes results over **MQTT**.

This project demonstrates a **modular edge AI pipeline** combined with an **event-driven IoT architecture**, designed for scalability and clean separation of concerns.

---

## ✨ Highlights

- 🧠 Edge AI inference on ESP32 (logistic regression / lightweight ML)
- 📡 MQTT-based telemetry (ESP32 → Broker → Python subscriber)
- 🧩 Modular pipeline (sensor → features → inference → decision)
- ⚡ Event-driven design with proper Wi-Fi → MQTT lifecycle
- ⚙️ Configurable via ESP-IDF `menuconfig`

---

## 🏗️ Architecture

```mermaid
flowchart LR
    A[Sensor Input (Simulated / HTTP)]
    B[app_sensor]
    C[app_window]
    D[app_features]
    E[app_inference]
    F[app_decision]
    G[app_pipeline]
    H[app_mqtt]
    I[MQTT Broker (Mosquitto)]
    J[Python Subscriber]

    A --> B --> C --> D --> E --> F
    F --> G
    G --> H
    H --> I
    I --> J
````

* ESP32 acts as a **producer (edge inference node)**
* Python acts as a **consumer (monitoring client)**
* MQTT broker routes messages between them

---

## 📡 MQTT Example

**Topic**

```
esp32/iot-controller/decision
```

**Payload**

```json
{
  "decision": "anomaly",
  "anomaly_score": 0.993,
  "anomaly_streak": 2,
  "backend": "logistic_regression",
  "is_anomaly": true,
  "sample_index": 42,
  "total_windows": 27
}
```

---

## ⚙️ Quick Start

### 1. Start MQTT Broker

```bash
mosquitto -c C:\mosquitto.conf -v
```

---

### 2. Run Python Subscriber

```bash
pip install paho-mqtt
python tools/mqtt_subscriber.py --broker <your-ip>
```

---

### 3. Configure & Flash ESP32

```bash
idf.py menuconfig
idf.py build flash monitor
```

Set:

```
MQTT Broker URI → mqtt://<your-ip>:1883
```

---

## 📊 Example Output

```
[esp32/iot-controller/decision]
{
  "decision": "normal",
  "anomaly_score": 0.03
}

[esp32/iot-controller/decision]
{
  "decision": "anomaly",
  "anomaly_score": 0.99
}
```

---

## 🧠 Design Highlights

### Decoupled Architecture

* `app_decision` is independent of MQTT
* `app_mqtt` handles transport only
* `app_pipeline` integrates the full processing flow

### Correct Network Lifecycle

MQTT starts only after:

```
Wi-Fi connected → IP_EVENT_STA_GOT_IP → MQTT start
```

This prevents connection errors such as:

```
connect() error: Host is unreachable
```

### Scalable IoT Pattern

* ESP32 = producer (edge node)
* Python = subscriber

This follows a standard **producer–consumer architecture**.

---

## 📘 Documentation

For detailed system design and implementation, see:

* [Architecture & Design](docs/architecture.md)

---

## 🎯 What This Project Demonstrates

* Embedded system architecture (ESP-IDF)
* Real-time data pipeline design
* Edge AI inference on MCU
* MQTT-based IoT communication
* Event-driven firmware design

---

## 🚀 Future Improvements

* MQTT QoS / retry mechanism
* Offline buffering (store & forward)
* Cloud integration (AWS IoT / Azure)
* Visualization dashboard

---

## 👨‍💻 Author

Wayne Chung
Embedded Linux / Firmware Engineer