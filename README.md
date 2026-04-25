| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Wi-Fi Station Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the Wi-Fi Station functionality of the Wi-Fi driver of ESP for connecting to an Access Point.

## How to use example

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set the Wi-Fi configuration.
    * Set `WiFi SSID`.
    * Set `WiFi Password`.

Optional: If you need, change the other options according to your requirements.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)

## Example Output
Note that the output, in particular the order of the output, may vary depending on the environment.

Console output if station connects to AP successfully:
```
I (589) wifi station: ESP_WIFI_MODE_STA
I (599) wifi: wifi driver task: 3ffc08b4, prio:23, stack:3584, core=0
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (629) wifi: wifi firmware version: 2d94f02
I (629) wifi: config NVS flash: enabled
I (629) wifi: config nano formating: disabled
I (629) wifi: Init dynamic tx buffer num: 32
I (629) wifi: Init data frame dynamic rx buffer num: 32
I (639) wifi: Init management frame dynamic rx buffer num: 32
I (639) wifi: Init management short buffer num: 32
I (649) wifi: Init static rx buffer size: 1600
I (649) wifi: Init static rx buffer num: 10
I (659) wifi: Init dynamic rx buffer num: 32
I (759) phy: phy_version: 4180, cb3948e, Sep 12 2019, 16:39:13, 0, 0
I (769) wifi: mode : sta (30:ae:a4:d9:bc:c4)
I (769) wifi station: wifi_init_sta finished.
I (889) wifi: new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1
I (889) wifi: state: init -> auth (b0)
I (899) wifi: state: auth -> assoc (0)
I (909) wifi: state: assoc -> run (10)
I (939) wifi: connected with #!/bin/test, aid = 1, channel 6, BW20, bssid = ac:9e:17:7e:31:40
I (939) wifi: security type: 3, phy: bgn, rssi: -68
I (949) wifi: pm start, type: 1

I (1029) wifi: AP's beacon interval = 102400 us, DTIM period = 3
I (2089) esp_netif_handlers: sta ip: 192.168.77.89, mask: 255.255.255.0, gw: 192.168.77.1
I (2089) wifi station: got ip:192.168.77.89
I (2089) wifi station: connected to ap SSID:myssid password:mypassword
```

Console output if the station failed to connect to AP:
```
I (589) wifi station: ESP_WIFI_MODE_STA
I (599) wifi: wifi driver task: 3ffc08b4, prio:23, stack:3584, core=0
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (629) wifi: wifi firmware version: 2d94f02
I (629) wifi: config NVS flash: enabled
I (629) wifi: config nano formating: disabled
I (629) wifi: Init dynamic tx buffer num: 32
I (629) wifi: Init data frame dynamic rx buffer num: 32
I (639) wifi: Init management frame dynamic rx buffer num: 32
I (639) wifi: Init management short buffer num: 32
I (649) wifi: Init static rx buffer size: 1600
I (649) wifi: Init static rx buffer num: 10
I (659) wifi: Init dynamic rx buffer num: 32
I (759) phy: phy_version: 4180, cb3948e, Sep 12 2019, 16:39:13, 0, 0
I (759) wifi: mode : sta (30:ae:a4:d9:bc:c4)
I (769) wifi station: wifi_init_sta finished.
I (889) wifi: new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1
I (889) wifi: state: init -> auth (b0)
I (1889) wifi: state: auth -> init (200)
I (1889) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (1889) wifi station: retry to connect to the AP
I (1899) wifi station: connect to the AP fail
I (3949) wifi station: retry to connect to the AP
I (3949) wifi station: connect to the AP fail
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

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.


## Reviw by Codex CLI

 Primary findings: no reverse data-flow violation is present in the TinyML path; the runtime chain in main/app_main.c:50 is strictly forward-only.
  The main production risks are ownership concentrated in main, a singleton sensor implementation, scattered tuning constants, and an inference
  boundary that is only partly opaque for future TFLM.

  ### Architecture Issues

  - Medium: main is both boot code and pipeline owner. main/app_main.c:31 allocates all stage state, runs the scheduler loop, and performs the final
    output log at main/app_main.c:75. The flow is one-way, but orchestration and output are not first-class components.
  - Medium: app_inference is the correct future TinyML insertion point, but the boundary is not fully opaque. components/app_inference/
    app_inference.h:8 exposes model_data and tensor_arena, and main owns the context at main/app_main.c:35. TFLM can fit here, but backend details
    already leak outside the component.
  - Medium: components/app_sensor/app_sensor.c:9 uses a hidden singleton s_sample_index. That makes app_sensor non-reentrant, non-multi-instance, and
    awkward for replay testing or multiple sensor channels.
  - Medium: Parameter ownership is scattered across modules: sensor pattern/anomaly shape in components/app_sensor/app_sensor.c:5, calibration
    constants in components/app_calibration/app_calibration.c:11, inference thresholds in components/app_inference/app_inference.c:51, and decision
    hysteresis in components/app_decision/app_decision.c:30. Tuning requires multi-file code edits.
  - Low: components/app_features/app_features.h:23 binds feature extraction to app_window_t rather than a generic sample span. Fine for v1, but it
    couples features to one buffering strategy and makes replay/DMA/offline adapters harder.

  ### Code Quality Issues

  - Low: Static task creation results are not checked in main/app_main.c:95 and components/app_event/app_event.c:70. Failure is unlikely, but silent
    if it happens.
  - Low: Header/include hygiene needs cleanup. components/app_wifi/wifi_manager.h:4 has no include guard or #pragma once, and several files rely on
    transitive headers or carry unused includes, e.g. main/app_main.c:9 and components/app_wifi/wifi_manager.c:1.
  - Medium: The pipeline is structurally testable from calibration onward, but there are no source-side unit tests in the repo for window ordering,
    feature math, inference thresholds, or decision hysteresis. For a production-bound anomaly pipeline, that is a real gap.

  ### Build / CMake Issues

  - The new pipeline dependency chain is clean and one-way at the component level: app_sensor, app_calibration, app_window, app_features,
    app_inference, app_decision, then main in main/CMakeLists.txt:1.
  - Low: components/app_event/CMakeLists.txt:1 overdeclares esp_event, esp_netif, and nvs_flash for what is now just a static queue/task wrapper.
    components/app_wifi/CMakeLists.txt:1 also stays broader than the current code shape suggests.
  - Low: main is the dependency sink at main/CMakeLists.txt:4. Acceptable for a first increment, but not ideal once more outputs or control paths are
    added.
  - Low: README.md:4 is stale and still describes a Wi‑Fi station example. I also did not find a project-owned HTTP server component in main/ or
    components/, despite the broader project context.
  - I did not run a full end-to-end idf.py build in this review; the CMake assessment above is from source/manifests and code structure.

  ### Suggested Improvements

  - Move pipeline ownership into an app_pipeline component so main returns to boot/init only.
  - Make app_sensor instance-based or introduce a source interface for sim, hw, and replay.
  - Keep app_feature_vector_t input and app_inference_result_t output stable, but hide backend-specific ML details behind a stricter inference init/
    config API.
  - Centralize tunables in one boundary: Kconfig, a config header, or a pipeline config struct.
  - Add unit tests for app_window, app_features, app_inference, and app_decision.
  - Update README/build docs to reflect the actual project architecture.

  ### Risk Level

  - Medium. The TinyML path is one-way, fixed-buffer, and heap-free in project-owned code, which is a good base. It is not yet production-hardened
    because of singleton sensor state, scattered tuning ownership, missing tests, and main/app_inference boundary leakage.

  ### ASCII Diagram

  Project Boot / Integration
  +------------------------------------------------------------------+
  | main/app_main.c                                                  |
  | - NVS init                                                       |
  | - app_event_init/start                                           |
  | - wifi_init_sta()                                                |
  | - app_pipeline_task()   <-- current pipeline owner               |
  +---------------------------+--------------------------------------+
                              |
                              +--> Existing connectivity path
                              |    [app_wifi] --app_event_t--> [app_event] --log-->
                              |
                              +--> TinyML one-way pipeline
                                   [app_sensor] sensor_read_sample()
                                     out: app_sensor_sample_t (raw)
                                          |
                                          v
                                   [app_calibration] calibration_apply()      [unit-testable]
                                     out: app_sensor_sample_t (calibrated)
                                          |
                                          v
                                   [app_window] window_push()/get()           [unit-testable]
                                     state: app_window_t { samples[16], ... }
                                     when full
                                          |
                                          v
                                   [app_features] feature_extract()           [unit-testable]
                                     out: app_feature_vector_t
                                          |
                                          v
                                   [app_inference] inference_run()            [unit-testable]
                                     in : app_feature_vector_t
                                     ctx: app_inference_context_t
                                          { model_data, tensor_arena, tensor_arena_size }
                                     out: app_inference_result_t
                                     future TFLM integration point
                                          |
                                          v
                                   [app_decision] decision_update()           [unit-testable]
                                     out: app_decision_state_t
                                          |
                                          v
                                   [output in main] ESP_LOGI
                                     uses: app_decision_state_t
                                           + app_inference_result_t

  ### Optional hooks:
  - Replay/debug source before app_sensor
  - Raw/calibrated/features/result logging between stages

  ### Mermaid Diagram

  ### flowchart LR
      subgraph Main["main component"]
          Boot["app_main()<br/>boot + task startup"]
          Pipe["app_pipeline_task()<br/>current pipeline owner"]
          Output["ESP_LOGI output<br/>(current output stage)"]
      end

      subgraph Wifi["existing connectivity path"]
          WifiMgr["app_wifi<br/>wifi_init_sta()"]
          AppEvt["app_event<br/>queue/task"]
      end

      subgraph Sensor["app_sensor<br/>stateful / partially unit-testable"]
          S["sensor_read_sample()<br/>-> app_sensor_sample_t"]
      end

      subgraph Cal["app_calibration<br/>unit-testable"]
          C["calibration_apply()<br/>app_sensor_sample_t -> app_sensor_sample_t"]
      end

      subgraph Win["app_window<br/>unit-testable"]
          W["window_push() / app_window_get()<br/>app_window_t { samples[16], next_index, count }"]
      end

      subgraph Feat["app_features<br/>unit-testable"]
          F["feature_extract()<br/>-> app_feature_vector_t"]
      end

      subgraph Infer["app_inference<br/>unit-testable / future TinyML boundary"]
          I["inference_run()<br/>app_feature_vector_t -> app_inference_result_t"]
          TFLM["Future TensorFlow Lite Micro<br/>model + tensor arena"]
      end

      subgraph Dec["app_decision<br/>unit-testable"]
          D["decision_update()<br/>-> app_decision_state_t"]
      end

      Boot --> WifiMgr
      Boot --> Pipe
      WifiMgr -->|app_event_t| AppEvt

      Pipe --> S
      S -->|raw app_sensor_sample_t| C
      C -->|calibrated app_sensor_sample_t| W
      W -->|window full: const app_window_t*| F
      F -->|app_feature_vector_t| I
      I -->|app_inference_result_t| D
      D -->|app_decision_state_t| Output
      I -->|app_inference_result_t| Output

      TFLM -. integrates inside .-> I
      Replay["Replay / simulated source"] -. inject before .-> S
      Trace["Logging / debug taps"] -. raw / calibrated / features / result .-> Pipe