| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ESP32 IoT Controller - Sensor Anomaly Detection System

An advanced IoT controller for ESP32 that performs real-time sensor anomaly detection using machine learning inference. The system collects sensor data (temperature and pressure), processes it through a feature extraction pipeline, and uses inference models to detect anomalous patterns.

## Overview

This project demonstrates a production-grade embedded ML application on ESP32, featuring:

- **Real-time Sensor Processing**: Continuous acquisition of temperature and pressure data
- **Feature Extraction**: Advanced signal processing to extract meaningful features from sensor windows
- **Machine Learning Inference**: Anomaly detection using configurable inference backends (rule-based, logistic regression, or TensorFlow Lite Micro)
- **Event-Driven Architecture**: Asynchronous event system for decoupled component communication
- **WiFi Connectivity**: Station mode connectivity for remote monitoring and control
- **Configurable Decision Logic**: Multi-state decision system (Normal → Monitor → Anomaly) with streak detection
- **Modular Design**: Well-isolated components for testability and reusability
- **Comprehensive Testing**: Integrated test framework for unit and integration testing

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
         ┌────────────────┼────────────────┐
         │                │                │
    ┌────▼────┐   ┌───────▼────────┐   ┌──▼──────────┐
    │  WiFi   │   │  Event System  │   │   Pipeline  │
    │ Manager │   │ (FreeRTOS)     │   │   (Core)    │
    └────────┘   └────────────────┘   └──────────────┘
                                               │
        ┌──────────────────────────────────────┼──────────────────────────────────┐
        │                                      │                                  │
   ┌────▼─────────┐   ┌───────────┐   ┌──────▼────────┐   ┌───────────┐   ┌────▼────┐
   │ app_sensor   │───│app_window │───│app_features   │───│app_infer- │───│app_deci-│
   │ (Sensor I/O) │   │ (Buffering)   │ (Feature      │   │ence       │   │sion     │
   │              │   │           │   │  Extraction)  │   │ (ML Model)│   │(Anomaly)│
   └──────────────┘   └───────────┘   └───────────────┘   └───────────┘   └─────────┘
```

## Component Breakdown

### Core Components

| Component | Purpose | Key Functions |
|-----------|---------|---|
| **app_sensor** | Raw sensor data acquisition | `app_sensor_init()`, `app_sensor_read_sample()` |
| **app_window** | Sliding window buffer management | `window_push()`, `app_window_is_full()` |
| **app_features** | Feature extraction from sensor windows | `feature_extract()` |
| **app_inference** | ML model inference execution | `app_inference_init()`, `inference_run()` |
| **app_decision** | State machine for anomaly decisions | `decision_update()`, `app_decision_state_to_string()` |
| **app_pipeline** | Orchestrates the complete processing pipeline | `app_pipeline_start()` |
| **app_event** | FreeRTOS event queue system | `app_event_post()`, `app_event_get()` |
| **app_wifi** | WiFi connectivity management | `wifi_init_sta()` |
| **app_config** | Configuration constants and calibration | Tunable parameters |

### Supporting Components

- **app_calibration**: Sensor calibration parameters (temperature/pressure scaling and offset)
- **app_test**: Comprehensive test suite
- **app_features**: Feature vector definitions and extraction logic

## Key Features

### Sensor Data Pipeline
- **Sampling Period**: 1000ms (configurable)
- **Window Capacity**: 16 samples
- **Sensor Types**: Temperature (°C) and Pressure (kPa)

### Feature Extraction
Six statistical features are extracted per window:
1. Temperature Mean
2. Temperature Range
3. Temperature Mean Delta
4. Pressure Mean
5. Pressure Range
6. Pressure Mean Delta

### Anomaly Detection States
```
NORMAL ──────► MONITOR ──────► ANOMALY
  ▲             (score > 0.45)   (score > 0.80)
  │                                    │
  └────────────────────────────────────┘
       (streak = 0, score < 0.45)
```

### Decision Thresholds (Configurable)
- **Monitor Score**: 0.45 - Triggers monitoring state
- **Anomaly Score**: 0.80 - Confirms anomaly detection
- **Anomaly Streak**: 2 - Consecutive anomaly windows required for confirmation

## Hardware Requirements

- **ESP32 Development Board** (or compatible target)
- **Sensor Module** (configurable for temperature/pressure readings)
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
│   ├── app_main.c
│   ├── CMakeLists.txt
│   └── Kconfig.projbuild
│
├── components/              # Core components
│   ├── app_sensor/          # Sensor data acquisition
│   ├── app_window/          # Sliding window buffer
│   ├── app_features/        # Feature extraction
│   ├── app_inference/       # ML model inference
│   ├── app_decision/        # Anomaly decision logic
│   ├── app_pipeline/        # Pipeline orchestration
│   ├── app_event/           # Event system
│   ├── app_wifi/            # WiFi management
│   ├── app_config/          # Configuration definitions
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

## Configuration

### WiFi Configuration

Edit the project configuration to set WiFi credentials:

```bash
idf.py menuconfig
```

Navigate to `Example Configuration` and set:
- **WiFi SSID**: Your network name
- **WiFi Password**: Your network password

### Application Configuration

Edit [components/app_config/app_config.h](components/app_config/app_config.h) to adjust:

```c
// Sensor sampling period (milliseconds)
#define APP_SENSOR_SAMPLE_PERIOD_MS        1000U

// Number of samples in rolling window
#define APP_WINDOW_CAPACITY                16U

// Sensor calibration
#define APP_CAL_TEMP_SCALE                 1.0f
#define APP_CAL_TEMP_OFFSET                0.0f
#define APP_CAL_PRESSURE_SCALE             1.0f
#define APP_CAL_PRESSURE_OFFSET            0.0f

// Decision thresholds
#define APP_DECISION_MONITOR_SCORE         0.45f
#define APP_DECISION_ANOMALY_SCORE         0.80f
#define APP_DECISION_ANOMALY_STREAK        2U

// Inference threshold
#define APP_INFER_ANOMALY_THRESHOLD        0.50f
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
- `score`: Anomaly score (0.0-1.0), where >0.80 indicates anomaly
- `decision`: Current state (normal/monitor/anomaly)
- `streak`: Consecutive anomaly windows detected
- `temp_mean`: Average temperature in current window (°C)
- `temp_range`: Temperature variation within window (°C)
- `pressure_mean`: Average pressure in current window (kPa)
- `pressure_range`: Pressure variation within window (kPa)
- `backend`: Inference backend used (logistic_regression, rule_engine, tflite)

## Running Tests

If test mode is enabled (CONFIG_APP_RUN_TESTS_ONLY):

```bash
# Configure project for tests
idf.py menuconfig
# Enable "Run tests only" under Application configuration

# Build and flash
idf.py build flash monitor
```

See [components/app_test/](components/app_test/) for test implementation details.

## Component Details

### Sensor Module (`app_sensor`)
- Provides sample_t structure containing temperature, pressure, and sample index
- Maintains internal state for reentrancy and testing
- Supports multiple sensor instances

### Window Module (`app_window`)
- Implements circular buffer for sensor samples
- Tracks number of samples and next write position
- Allows ordered access to buffered samples

### Feature Extraction (`app_features`)
- Computes 6 statistical features from sensor windows
- Features normalize data for ML model input
- Supports reentrant operation

### Inference Module (`app_inference`)
- Pluggable backend architecture (rule-based, regression, TensorFlow Lite)
- Returns anomaly score and classification result
- Backend selection at compile-time

### Decision Module (`app_decision`)
- Three-state machine: NORMAL → MONITOR → ANOMALY
- Streak-based confirmation to reduce false positives
- Tracks statistics: total windows, anomaly windows, streak count

## Troubleshooting

### Understanding Pipeline Output

The pipeline logs provide real-time insight into anomaly detection. Here's how to interpret patterns:

**Normal Operation (score < 0.45):**
```
sample=41 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30
```
- Low score (0.03) indicates sensor readings are within expected range
- Small temperature and pressure ranges show stable conditions
- Streak remains at 0 (no anomalies detected)

**Anomaly Detection Sequence:**
```
sample=42 score=0.99 decision=anomaly streak=1  temp_mean=25.04 temp_range=3.55 pressure_mean=101.02 pressure_range=2.80
sample=43 score=1.00 decision=anomaly streak=2  temp_mean=25.29 temp_range=4.25 pressure_mean=100.82 pressure_range=3.30
sample=44 score=1.00 decision=anomaly streak=3  temp_mean=25.56 temp_range=4.40 pressure_mean=100.61 pressure_range=3.45
```
- Score jumps from 0.03 to 0.99+ indicating anomalous pattern detected
- Streak counter increments for each consecutive anomalous window
- Temperature and pressure ranges significantly increase (3-5x larger)
- This could indicate a real anomaly or simulated/injected test pattern

**Return to Normal:**
```
sample=63 score=0.03 decision=normal streak=0 temp_mean=24.82 temp_range=0.35 pressure_mean=101.19 pressure_range=0.30
```
- Score drops back to 0.03
- Streak resets to 0
- System returns to normal monitoring

### WiFi Connection Issues
- Verify SSID and password in menuconfig
- Check WiFi signal strength in your location
- Review logs for WiFi errors

### Build Failures
- Ensure ESP-IDF environment is sourced: `. $IDF_PATH/export.sh`
- Clean build: `idf.py fullclean && idf.py build`
- Check Python version: `python --version` (should be 3.7+)

### Serial Monitor Not Detecting Device
- Verify USB cable connection
- Install/update CH340 drivers (common on ESP32 boards)
- Check device manager for correct COM port

### No Anomalies Detected
- Verify sensor is properly connected and generating varied data
- Check inference thresholds in `app_config.h` (may need tuning)
- Inject test patterns to validate detection logic
- Review ML model backend selection

## API Reference

### Pipeline Initialization
```c
void app_pipeline_start(void);  // Start the main processing loop
```

### Event System
```c
void app_event_init(void);
bool app_event_post(app_event_t event);
bool app_event_get(app_event_t *event, TickType_t ticks_to_wait);
void app_event_start(void);
```

### Sensor Reading
```c
void app_sensor_init(app_sensor_t *sensor);
bool app_sensor_read_sample(app_sensor_t *sensor, app_sensor_sample_t *sample);
```

### Feature Extraction
```c
bool feature_extract(const app_window_t *window, app_feature_vector_t *features);
```

### Inference
```c
void app_inference_init(void);
bool inference_run(const app_feature_vector_t *features, app_inference_result_t *result);
```

## References

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/)
- [ESP32 Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
- [FreeRTOS on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html)

## License

See LICENSE file (if applicable).

## Contributing

Contributions are welcome! Please ensure:
- Code follows project style guidelines
- All tests pass
- New features include documentation updates

## Support

For issues and questions:
- Check existing GitHub issues
- Review project documentation
- Consult ESP-IDF documentation
I (4069) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (4069) wifi: state: init -> auth (b0)
I (5069) wifi: state: auth -> init (200)
I (5069) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (5069) wifi station: retry to connect to the AP
I (5069) wifi station: connect to the AP fail
I (7129) wifi station: retry to connect to the AP
I (7129) wifi station: connect to the AP fail
I (7249) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (7249) wifi: state: init -> auth (b0)
I (8249) wifi: state: auth -> init (200)
I (8249) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (8249) wifi station: retry to connect to the AP
I (8249) wifi station: connect to the AP fail
I (10299) wifi station: connect to the AP fail
I (10299) wifi station: Failed to connect to SSID:myssid, password:mypassword
```

```
## Python Integration Test (HTTP Sensor Input)

This project includes a Python-based integration test to validate the HTTP sensor input pipeline end-to-end.

The test simulates sensor data from a host machine and sends it to the ESP32 over HTTP:

```

Python → HTTP POST /sensor → ESP32 → Queue → Pipeline → Inference → Decision

```

### Location

```

tests/integration/http_sensor_test.py

````

---

### Prerequisites

Install Python dependency:

```bash
pip install requests
````

---

### Usage

```bash
python tests/integration/http_sensor_test.py <ESP32_IP> --mode <mode>
```

Example:

```bash
python tests/integration/http_sensor_test.py 192.168.8.88 --mode mixed
```

---

### Test Modes

#### 1. Normal Mode

```bash
--mode normal
```

* Sends stable sensor values with small noise
* Expected result: `decision=normal`
* Validates baseline behavior

---

#### 2. Anomaly Mode

```bash
--mode anomaly
```

* Sends extreme sensor values
* Expected result: `decision=anomaly`
* Validates anomaly detection sensitivity

---

#### 3. Mixed Mode (Recommended)

```bash
--mode mixed
```

* Pattern:

  ```
  20 normal → 20 anomaly → 25 normal
  ```
* Designed for a 16-sample window
* Validates:

  * Detection transition (normal → anomaly)
  * Recovery behavior (anomaly → normal)

---

#### 4. Stress Mode

```bash
--mode stress
```

* Sends data at high frequency
* Validates:

  * Queue behavior
  * Backpressure handling
  * System stability under load

---

### Expected Output (ESP32 Logs)

Normal operation:

```
decision=normal score≈0.05
temp_range≈0.3 pressure_range≈0.5
```

Anomaly detected:

```
decision=anomaly score≈1.00
temp_range≫10 pressure_range≫10
```

Recovery:

```
decision=normal (after enough normal samples)
```

---

### Important Notes

* The system uses a **16-sample sliding window**
* A single anomaly sample can significantly affect:

  ```
  temp_range / pressure_range
  ```
* Therefore:

  * Model decision reacts immediately
  * System stability should be handled via **streak / alarm logic**

---

### Testing Strategy

To fully validate the system:

1. Run `normal` → verify no false positives
2. Run `anomaly` → verify detection triggers
3. Run `mixed` → verify transition and recovery
4. Run `stress` → verify system robustness

---

### Troubleshooting

If requests fail:

* Verify ESP32 IP address from serial monitor
* Ensure WiFi connection is established
* Check HTTP server is started:

  ```
  app_http: HTTP server started, POST /sensor
  ```

---

### Notes for Development

This Python test acts as an **integration test tool**, not a unit test.

It validates:

* Network stack (WiFi + HTTP)
* Data ingestion
* Pipeline processing
* ML inference behavior

This makes it useful for:

* Debugging pipeline issues
* Validating model behavior
* Demonstrating system-level design in interviews

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.