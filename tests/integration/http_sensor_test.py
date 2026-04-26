import sys
import time
import random
import requests

if len(sys.argv) < 2:
    print("Usage: python http_sensor_test.py <ESP32_IP> [--mode normal|anomaly|mixed|stress]")
    exit(1)

esp32_ip = sys.argv[1]
mode = "mixed"

if len(sys.argv) >= 4 and sys.argv[2] == "--mode":
    mode = sys.argv[3]

url = f"http://{esp32_ip}/sensor"

SEND_INTERVAL_SEC = 1.2

def build_payload(expected):
    if expected == "anomaly":
        return {
            "temperature_c": random.choice([38.0, 42.0, 10.0]),
            "pressure_kpa": random.choice([85.0, 125.0, 140.0]),
        }
    else:
        return {
            "temperature_c": 25.0 + random.uniform(-0.2, 0.2),
            "pressure_kpa": 101.3 + random.uniform(-0.3, 0.3),
        }

def get_pattern(mode):
    if mode == "normal":
        return ["normal"] * 40

    elif mode == "anomaly":
        return ["anomaly"] * 40

    elif mode == "stress":
        global SEND_INTERVAL_SEC
        SEND_INTERVAL_SEC = 0.2
        return ["normal"] * 50

    elif mode == "mixed":
        # Base on the 16 smpale windows in app_sensor.c
        return (
            ["normal"] * 20 +
            ["anomaly"] * 20 +
            ["normal"] * 25
        )

    else:
        print(f"Unknown mode: {mode}")
        exit(1)

pattern = get_pattern(mode)

print(f"Running mode: {mode}, total samples: {len(pattern)}")

for i, expected in enumerate(pattern):
    payload = build_payload(expected)

    try:
        r = requests.post(url, json=payload, timeout=3)
        print(
            f"{i:02d} expected={expected:7s} "
            f"temp={payload['temperature_c']:.2f} "
            f"pressure={payload['pressure_kpa']:.2f} "
            f"status={r.status_code}"
        )
    except requests.exceptions.RequestException as e:
        print(f"{i:02d} request failed: {e}")

    time.sleep(SEND_INTERVAL_SEC)