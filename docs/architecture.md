# esp32-edge-ai-sensor-controller
Modular ESP32 edge AI controller with pluggable sensor inputs (simulated/HTTP) and real-time anomaly detection pipeline.
| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ESP32 IoT Controller - Sensor Anomaly Detection System

An advanced IoT controller for ESP32 that performs real-time sensor anomaly detection using machine learning inference. The system collects sensor data (temperature and pressure), processes it through a feature extraction pipeline, and uses inference models to detect anomalous patterns.

## ⚠️ Project Status: MVP/Proof-of-Concept

This is a **minimal viable product** designed for prototyping and development. Key limitations:

- **ML Model**: Uses logistic regression with **placeholder weights** - must be replaced with trained model for production
- **Inference Backend**: Currently **logistic regression only** (TensorFlow Lite and rule-based backends are not implemented)
- **Security**: Unauthenticated HTTP, no HTTPS/TLS (see Security Notice below)
- **Configuration**: Compile-time only (no runtime tuning via menuconfig)
- **Queue Capacity**: Limited to 8 samples in HTTP mode (data may be dropped under high load)

## Overview

This project demonstrates a modular embedded ML application on ESP32, featuring:

- **Real-time Sensor Processing**: Continuous acquisition of temperature and pressure data
- **Feature Extraction**: Advanced signal processing to extract meaningful features from sensor windows
- **Machine Learning Inference**: Anomaly detection using logistic regression (MVP model with placeholder weights)
- **Flexible Sensor Input**: Simulated sensor data or HTTP REST API for remote data injection
- **Event-Driven Architecture**: Asynchronous event system for decoupled component communication
- **WiFi Connectivity**: Station mode connectivity for remote monitoring and control
- **MQTT Publishing**: Real-time anomaly event publishing (partial implementation)
- **Configurable Decision Logic**: Multi-state decision system (Normal → Monitor → Anomaly) with streak detection
- **Modular Design**: Well-isolated components for testability and reusability
- **Comprehensive Testing**: Integrated test framework for unit and integration testing

## ⚠️ Security Notice

This is an **MVP/proof-of-concept implementation** designed for local testing and development. The following limitations should be understood:

- **HTTP endpoint is unauthenticated**: The `/sensor` endpoint accepts requests from any device on the local network without authentication. This is suitable for testing but not for production deployment.
- **No HTTPS/TLS support**: Communication is unencrypted plaintext.
- **No input validation**: Limited validation of incoming data
- **Local network only**: Intended for use on trusted local networks during development.

**For production deployment**, implement:
- ✅ API authentication (API key, JWT, or mutual TLS)
- ✅ HTTPS/TLS encryption
- ✅ Input validation and rate limiting
- ✅ Trained ML model (replace placeholder weights)
- ✅ Security audit and hardening
- ✅ Production logging and monitoring

Do not expose this device to untrusted networks or the public internet without these security measures.

## Supported Targets

- ESP32
- ESP32-S2
- ESP32-S3
- ESP32-C2
- ESP32-C3
- ESP32-C6

## System Architecture

```
┌──────────────────────────────────────────────────────┐
│                   app_main.c                         │
│  Initialization & Task Orchestration                 │
└──────────────────────────────────────────────────────┘
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
        ▼                 ▼                 ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  WiFi Setup  │  │  HTTP Server │  │  Event Init  │
│  (STA mode)  │  │  (optional)  │  │              │
└──────────────┘  └──────────────┘  └──────────────┘
                          │
                          ▼
                ┌──────────────────┐
                │   Pipeline Task  │
                │  (FreeRTOS)      │
                └──────────────────┘
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
        ▼                 ▼                 ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  Sensor      │  │  Calibration │  │  Sliding     │
│  Input       │  │  (linear     │  │  Window (16) │
│  Simulation  │  │   transform) │  │              │
│  or HTTP     │  │              │  │              │
└──────────────┘  └──────────────┘  └──────────────┘
                          │
                          ▼
                ┌──────────────────┐
                │ Feature Extract  │
                │ (6 statistics)   │
                └──────────────────┘
                          │
                          ▼
                ┌──────────────────┐
                │ Inference        │
                │ (Logistic Reg)   │
                │ [MVP Weights]    │
                └──────────────────┘
                          │
                          ▼
                ┌──────────────────┐
                │ Decision State   │
                │ (N→M→A + streak) │
                └──────────────────┘
                          │
                          ▼
                ┌──────────────────┐
                │ MQTT Publish     │
                │ (optional)       │
                └──────────────────┘
```

## Component Breakdown

### Core Components

| Component | Purpose | Key Functions |
|-----------|---------|---|
| **app_sensor** | Raw sensor data acquisition (simulated or HTTP) | `app_sensor_init()`, `app_sensor_read_sample()` |
| **app_window** | Sliding window buffer management | `window_push()`, `app_window_is_full()` |
| **app_features** | Feature extraction from sensor windows (6 statistics) | `feature_extract()` |
| **app_inference** | Logistic regression inference (MVP model with placeholder weights) | `app_inference_init()`, `inference_run()` |
| **app_decision** | State machine for anomaly decisions (Normal/Monitor/Anomaly) | `decision_update()`, `app_decision_state_to_string()` |
| **app_pipeline** | Orchestrates the complete processing pipeline | `app_pipeline_start()` |
| **app_pipeline_queue** | FreeRTOS queue for sensor sample buffering | `app_pipeline_queue_init()`, `app_pipeline_queue_get()` |
| **app_event** | FreeRTOS event queue system | `app_event_post()`, `app_event_get()` |
| **app_http** | HTTP REST API server (sensor data injection) | HTTP `/sensor` endpoint |
| **app_wifi** | WiFi connectivity management | `wifi_init_sta()` |
| **app_config** | Configuration constants | Tunable parameters (compile-time) |

### Supporting Components

- **app_calibration**: Sensor calibration parameters (temperature/pressure linear scaling)
- **app_test**: Comprehensive test suite
- **app_mqtt**: MQTT event publishing (partial implementation)

## Key Features

### Sensor Data Pipeline
- **Sampling Period**: 1000ms (hardcoded, not configurable)
- **Window Capacity**: 16 samples
- **Sensor Types**: Temperature (°C) and Pressure (kPa)
- **Input Methods**: Simulated data or HTTP REST API

### Feature Extraction
Six statistical features are extracted per window:
1. Temperature Mean
2. Temperature Range (max - min)
3. Temperature Mean Delta (change rate)
4. Pressure Mean
5. Pressure Range (max - min)
6. Pressure Mean Delta (change rate)

### Anomaly Detection States
```
NORMAL ──────► MONITOR ──────► ANOMALY
  ▲             (score > 0.45)   (score > 0.80)
  └─────────────────────────────────┘
         (score < 0.45 after streak)
```

### Decision Thresholds (Compile-time Only)
- **Monitor Score**: 0.45 - Triggers monitoring state
- **Anomaly Score**: 0.80 - Requires 2 consecutive anomalies for ANOMALY state
- **Anomaly Streak**: 2 - Consecutive anomaly windows required for confirmation

### Inference Backend
- **Current**: Logistic Regression (MVP implementation with placeholder weights)
- **Normalization**: Features normalized before scoring (mean/stddev)
- **Score Range**: 0.0 - 1.0 (probability)

## Sensor Input Modes

### Mode 1: Simulated Sensor (Default)
- Generates synthetic temperature/pressure data
- Repeating normal pattern with periodic anomalies
- Useful for testing and demonstration

### Mode 2: HTTP REST API
Inject sensor data remotely via HTTP POST requests.

**Endpoint**: `POST /sensor`

**Supported Formats:**

**JSON Format:**
```json
{
  "temperature_c": 24.5,
  "pressure_kpa": 101.3
}
```

**CSV Format:**
```
24.5,101.3
```

**Response:** Plain text "ok" (HTTP 200) or error

**Example (using curl):**
```bash
curl -X POST http://<ESP32_IP>:80/sensor \
  -H "Content-Type: application/json" \
  -d '{"temperature_c": 24.5, "pressure_kpa": 101.3}'
```

**Queue Details:**
- Queue capacity: 8 samples maximum
- FIFO buffering for HTTP input
- Data dropped if queue full during high load

## Configuration Guide

### Compile-Time Configuration

All configuration is **compile-time only**. Edit [components/app_config/app_config.h](components/app_config/app_config.h):

```c
// Sensor sampling period (milliseconds) - HARDCODED
#define APP_SENSOR_SAMPLE_PERIOD_MS        1000U

// Number of samples in rolling window
#define APP_WINDOW_CAPACITY                16U

// Sensor calibration (linear transform: calibrated = raw * scale + offset)
#define APP_CAL_TEMP_SCALE                 1.0f      // Currently disabled
#define APP_CAL_TEMP_OFFSET                0.0f
#define APP_CAL_PRESSURE_SCALE             1.0f      // Currently disabled
#define APP_CAL_PRESSURE_OFFSET            0.0f

// Decision thresholds
#define APP_DECISION_MONITOR_SCORE         0.45f
#define APP_DECISION_ANOMALY_SCORE         0.80f
#define APP_DECISION_ANOMALY_STREAK        2U        // Consecutive anomalies

// Inference threshold
#define APP_INFER_ANOMALY_THRESHOLD        0.50f
```

**Note**: The `menuconfig` command does NOT affect application parameters - it only configures ESP-IDF build options. All application tuning requires recompilation.

### WiFi Configuration

Edit the project configuration to set WiFi credentials:

```bash
idf.py menuconfig
```

Navigate to `Example Configuration` and set:
- **WiFi SSID**: Your network name
- **WiFi Password**: Your network password

Then rebuild and flash.

## Hardware Requirements

- **ESP32 Development Board** (or compatible target)
- **Sensor Module** (if using real sensors - currently simulated)
- **Power Supply**: 5V USB or external power
- **Serial Interface**: USB-to-UART adapter for monitoring (optional)

## Software Requirements

- **ESP-IDF**: v4.4 or later
  - Download: https://github.com/espressif/esp-idf
- **Python 3.7+**: For ESP-IDF tools
- **C Compiler**: GCC/Clang (provided by ESP-IDF)
- **CMake**: v3.16 or later

## Project Structure

```
esp32-iot-controller/
├── README.md                 # This file
├── CMakeLists.txt           # Project build configuration
├── sdkconfig                # Build configuration (generated)
├── sdkconfig.defaults       # Default build settings
├── Kconfig.projbuild        # Project-specific Kconfig
│
├── main/                    # Application entry point
│   ├── app_main.c           # Init and task management
│   ├── CMakeLists.txt
│   └── Kconfig.projbuild
│
├── components/              # Core components
│   ├── app_sensor/          # Sensor data acquisition
│   ├── app_window/          # Sliding window buffer
│   ├── app_features/        # Feature extraction
│   ├── app_inference/       # ML model inference (logistic regression)
│   ├── app_decision/        # Anomaly decision logic (3-state machine)
│   ├── app_pipeline/        # Pipeline orchestration
│   ├── app_pipeline_queue/  # Sensor sample queue buffering
│   ├── app_event/           # Event system
│   ├── app_http/            # HTTP REST API server
│   ├── app_wifi/            # WiFi management
│   ├── app_mqtt/            # MQTT publishing (partial)
│   ├── app_config/          # Configuration definitions (compile-time)
│   ├── app_calibration/     # Sensor calibration
│   └── app_test/            # Test suite
│
└── build/                   # Build output (generated)
```

## Installation & Setup

### 1. Prerequisites

Ensure you have ESP-IDF installed and properly configured:

```bash
# Clone ESP-IDF repository (if not already done)
git clone --recursive https://github.com/espressif/esp-idf.git

# Navigate to ESP-IDF directory and install
cd esp-idf
./install.sh

# Set up environment (run this in your project terminal)
source ./export.sh  # On Linux/macOS
# OR
.\export.ps1        # On Windows PowerShell
```

### 2. Clone the Project

```bash
git clone <repository-url> esp32-iot-controller
cd esp32-iot-controller
```

### 3. Set Target Chip

```bash
idf.py set-target esp32  # Or your target: esp32s3, esp32c3, etc.
```

## Building and Flashing

### Build the Project

```bash
idf.py build
```

Or using VS Code (if ESP-IDF extension is installed):
- Press `Ctrl+Shift+P` → "ESP-IDF: Build your project"

### Flash to Device

```bash
# Find your device port (COM3 on Windows, /dev/ttyUSB0 on Linux, etc.)
idf.py -p PORT flash

# Or with automatic port detection
idf.py flash
```

### Build and Flash in One Step

```bash
idf.py -p PORT build flash monitor
```

## Monitoring

### View Serial Output

```bash
idf.py -p PORT monitor
```

Or in VS Code:
- Press `Ctrl+Shift+P` → "ESP-IDF: Monitor your device"

**Exit Monitor**: Press `Ctrl+]`

### Example Output

**Startup and WiFi Connection:**
```
I (589) wifi station: ESP_WIFI_MODE_STA
I (599) wifi: wifi driver task: 3ffc08b4, prio:23, stack:3584, core=0
I (629) wifi: Init data frame dynamic rx buffer num: 32
...
I (2089) esp_netif_handlers: sta ip: 192.168.77.89, mask: 255.255.255.0, gw: 192.168.77.1
I (2089) wifi station: got ip:192.168.77.89
```

**Pipeline Processing Output (Normal → Anomaly Detection → Normal):**

The pipeline logs each processed window with comprehensive sensor and inference data:

```
I (15681) app_pipeline: sample=15 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30 backend=logistic_regression
I (16681) app_pipeline: sample=16 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30 backend=logistic_regression
I (17681) app_pipeline: sample=17 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30 backend=logistic_regression
...
I (41681) app_pipeline: sample=41 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30 backend=logistic_regression

# Anomaly detected! Score jumps to 0.99+
I (42681) app_pipeline: sample=42 score=0.99 decision=anomaly streak=1 temp_mean=25.04 temp_range=3.55 pressure_mean=101.02 pressure_range=2.80 backend=logistic_regression
I (43681) app_pipeline: sample=43 score=1.00 decision=anomaly streak=2 temp_mean=25.29 temp_range=4.25 pressure_mean=100.82 pressure_range=3.30 backend=logistic_regression
I (44681) app_pipeline: sample=44 score=1.00 decision=anomaly streak=3 temp_mean=25.56 temp_range=4.40 pressure_mean=100.61 pressure_range=3.45 backend=logistic_regression
I (45681) app_pipeline: sample=45 score=1.00 decision=anomaly streak=4 temp_mean=25.85 temp_range=4.95 pressure_mean=100.38 pressure_range=4.00 backend=logistic_regression
I (46681) app_pipeline: sample=46 score=1.00 decision=anomaly streak=5 temp_mean=26.17 temp_range=5.10 pressure_mean=100.13 pressure_range=4.10 backend=logistic_regression
...
I (62681) app_pipeline: sample=62 score=1.00 decision=anomaly streak=21 temp_mean=25.17 temp_range=5.70 pressure_mean=100.93 pressure_range=4.50 backend=logistic_regression

# Anomaly ends, back to normal
I (63681) app_pipeline: sample=63 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30 backend=logistic_regression
I (64681) app_pipeline: sample=64 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30 backend=logistic_regression
```

**Output Field Descriptions:**
- `sample`: Sequential sample/window number
- `score`: Anomaly score (0.0-1.0), where >0.80 indicates anomaly (with 2-streak confirmation)
- `decision`: Current state (normal/monitor/anomaly)
- `streak`: Consecutive anomaly windows detected
- `temp_mean`: Average temperature in current window (°C)
- `temp_range`: Temperature variation within window (°C)
- `pressure_mean`: Average pressure in current window (kPa)
- `pressure_range`: Pressure variation within window (kPa)
- `backend`: Inference backend used (currently: logistic_regression only)

## Running Tests

If test mode is enabled (CONFIG_APP_RUN_TESTS_ONLY):

```bash
# Configure project for tests
idf.py menuconfig
# Enable "Run tests only" under Application configuration

# Build and flash
idf.py build flash monitor
```

## Production Deployment Roadmap

### Critical Before Production:
- [ ] **Replace ML Model**: Train logistic regression model with real sensor data and replace placeholder weights in `app_inference.c`
- [ ] **Add Authentication**: Implement API key or JWT validation for `/sensor` endpoint
- [ ] **Enable HTTPS**: Add TLS/SSL certificate support
- [ ] **Input Validation**: Validate and sanitize all HTTP inputs
- [ ] **Queue Management**: Monitor and handle queue overflow conditions
- [ ] **Error Handling**: Comprehensive error recovery and logging

### Optional Enhancements:
- [ ] Implement TensorFlow Lite backend for more complex models
- [ ] Add rule-based decision engine as alternative backend
- [ ] Complete MQTT implementation
- [ ] Runtime configuration via EEPROM or NVS
- [ ] Add real sensor driver integration
- [ ] Implement OTA firmware updates

## Troubleshooting

### Common Issues

**No WiFi Connection:**
- Verify SSID and password in `sdkconfig` via `idf.py menuconfig`
- Check WiFi router range and signal strength
- Ensure target device supports 2.4GHz (some ESP32 variants support 5GHz only)

**HTTP Endpoint Not Responding:**
- Verify ESP32 IP address from serial monitor output
- Ensure HTTP server is enabled in configuration
- Check firewall rules on your network
- Test with curl: `curl http://192.168.1.x:80/sensor`

**Pipeline Not Processing Data:**
- Check that sensor mode is enabled (not in test-only mode)
- Verify pipeline task has sufficient stack (check FreeRTOS task configuration)
- Monitor queue depth for overflow conditions

## Next Steps for Development

1. **Model Training**: Collect real sensor data and train logistic regression model
2. **Security Hardening**: Add authentication and HTTPS support
3. **Real Sensor Integration**: Replace simulated sensor with actual hardware driver
4. **Testing**: Expand test coverage for edge cases and stress conditions
5. **Production Deployment**: Follow deployment roadmap above

## Resources

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [FreeRTOS on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_isr_priorities.html)
- [ESP32 Hardware Reference](https://docs.espressif.com/projects/esp32-technical-reference-manual/en/latest/)
