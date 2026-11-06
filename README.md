# IoT-Based Industrial Temperature & Humidity Monitoring System Using ESP32

## 📌 Project Overview

This project implements an IoT-based Industrial Temperature and Humidity Monitoring System using an ESP32 microcontroller and a DHT22 sensor.

The DHT22 sensor continuously measures temperature and humidity. The ESP32 collects the sensor readings, connects to Wi-Fi, and publishes the data to an MQTT broker.

Using the HiveMQ WebSocket Client, users can monitor real-time temperature and humidity readings remotely.

The system also includes an LED and a push button for basic control functionality.

## 🎯 Objectives

- Measure temperature and humidity using the DHT22 sensor.
- Connect ESP32 to Wi-Fi.
- Publish sensor data using the MQTT protocol.
- Monitor real-time sensor readings through HiveMQ.
- Implement automatic Wi-Fi reconnection.
- Implement MQTT reconnection.
- Control an LED using a push button.
- Test online and offline connectivity.

## 🛠️ Components Required

- 1 × ESP32 Development Board
- 1 × DHT22 Temperature and Humidity Sensor
- 1 × LED
- 1 × 220Ω Resistor
- 1 × Push Button
- Jumper Wires

## 🔌 Pin Configuration

| Component | ESP32 Pin |
|---|---|
| DHT22 VCC | 3V3 |
| DHT22 DATA | GPIO 4 |
| DHT22 GND | GND |
| LED Anode | GPIO 19 (through resistor) |
| LED Cathode | GND |
| Push Button | GPIO 18 |
| Push Button Other Terminal | GND |

## ⚙️ Working Principle

1. The DHT22 sensor measures temperature and humidity.
2. The ESP32 reads sensor data at regular intervals.
3. The ESP32 connects to Wi-Fi using the Wokwi Public IoT Gateway.
4. The ESP32 establishes a connection with the MQTT broker.
5. Temperature and humidity readings are converted into JSON format.
6. The ESP32 publishes the sensor data to the MQTT topic.
7. The HiveMQ WebSocket Client subscribes to the same topic.
8. Published sensor readings appear in the Messages section of HiveMQ.
9. The push button controls the LED.
10. The system attempts to reconnect automatically if Wi-Fi or MQTT connectivity is lost.

## 📊 Expected Output

**Example Sensor Readings:**

| Parameter | Example Value |
|---|---|
| Temperature | 24.00 °C |
| Humidity | 40.00% |
| Wi-Fi Status | Connected |
| MQTT Status | Connected |

**Example MQTT JSON Message:**

```json
{
  "temperature": 24.00,
  "humidity": 40.00
}
```

## 💻 Source Code

The complete Arduino source code is available in:

`sketch.ino`

## 🎥 Simulation & Demonstration Links

- **Wokwi Simulation:** https://wokwi.com/projects/479599653547906049
- **Google Drive Demo Video:** PASTE_YOUR_GOOGLE_DRIVE_LINK
- **HiveMQ WebSocket Client:** https://www.hivemq.com/demos/websocket-client/

**Required Libraries:**

- DHT Sensor Library
- PubSubClient Library

## 🌐 Wi-Fi Configuration

| Parameter | Value |
|---|---|
| Wi-Fi SSID | Wokwi-GUEST |
| Password | Empty |
| Gateway | Wokwi Public IoT Gateway |

## 📡 MQTT Configuration

The project uses the public HiveMQ MQTT broker for communication.

| Parameter | Value |
|---|---|
| MQTT Broker | broker.hivemq.com |
| ESP32 MQTT Port | 1883 |
| Protocol | MQTT |
| Data Format | JSON |
| QoS | 0 |
| ClientID | mukta_esp32_test01 |

### MQTT Topic

**Publishing Topic:**

`mec/cse08/industrial-monitor/temperature`

**Subscription Topic:**

`mec/cse08/industrial-monitor/temperature`

The publishing and subscription topics must be identical to receive the sensor data.

## 📚 Technologies Used

- ESP32
- Arduino C++
- DHT22 Sensor
- Wi-Fi Communication
- MQTT Protocol
- PubSubClient Library
- HiveMQ MQTT Broker
- Wokwi Simulator

**Conclusion**

This project demonstrates an IoT-based monitoring system that collects temperature and humidity data using the DHT22 sensor and transmits the readings through MQTT communication. It also demonstrates Wi-Fi connectivity, real-time data monitoring, and automatic network reconnection.

Developed as part of an Embedded Systems / IoT task.
