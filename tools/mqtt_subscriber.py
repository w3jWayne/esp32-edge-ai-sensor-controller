#!/usr/bin/env python3
import argparse
import json
import sys
import threading

import paho.mqtt.client as mqtt


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Subscribe to ESP32 decision messages over MQTT."
    )
    parser.add_argument(
        "--broker",
        default="localhost",
        help="MQTT broker hostname or IP address (default: localhost)",
    )
    parser.add_argument(
        "--port",
        type=int,
        default=1883,
        help="MQTT broker port (default: 1883)",
    )
    parser.add_argument(
        "--topic",
        default="esp32/iot-controller/decision",
        help="MQTT topic to subscribe to (default: esp32/iot-controller/decision)",
    )
    return parser.parse_args()


def on_connect(client, userdata, flags, reason_code, properties):
    del flags
    del properties

    if reason_code.is_failure:
        print(f"Failed to connect to MQTT broker: {reason_code}", file=sys.stderr)
        return

    topic = userdata["topic"]
    print(f"Connected to MQTT broker at {userdata['broker']}:{userdata['port']}")
    client.subscribe(topic)
    print(f"Subscribed to topic: {topic}")


def on_message(client, userdata, msg):
    del client
    del userdata

    payload_text = msg.payload.decode("utf-8", errors="replace")

    try:
        payload = json.loads(payload_text)
    except json.JSONDecodeError:
        print(f"[{msg.topic}] {payload_text}")
        return

    print(f"[{msg.topic}]")
    print(json.dumps(payload, indent=2, sort_keys=True))


def on_disconnect(client, userdata, disconnect_flags, reason_code, properties):
    del client
    del userdata
    del disconnect_flags
    del properties

    if reason_code.is_failure:
        print(f"Disconnected unexpectedly: {reason_code}", file=sys.stderr)


def main() -> int:
    args = parse_args()
    client = mqtt.Client(
        mqtt.CallbackAPIVersion.VERSION2,
        userdata={
            "broker": args.broker,
            "port": args.port,
            "topic": args.topic,
        },
    )
    stop_event = threading.Event()

    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect

    try:
        client.connect(args.broker, args.port, keepalive=60)
    except OSError as exc:
        print(f"Could not connect to {args.broker}:{args.port}: {exc}", file=sys.stderr)
        return 1

    client.loop_start()

    try:
        while not stop_event.wait(1.0):
            pass
    except KeyboardInterrupt:
        print("\nStopping MQTT subscriber...")
    finally:
        client.disconnect()
        client.loop_stop()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
