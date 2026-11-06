
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ---------- PIN CONFIGURATION ----------

#define DHT_PIN 4
#define DHT_TYPE DHT22

#define BUTTON_PIN 18
#define LED_PIN 19

// ---------- WIFI CONFIGURATION ----------

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ---------- MQTT CONFIGURATION ----------

const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

const char* TOPIC_TEMP =
  "mec/cse08/industrial-monitor/temperature";

const char* TOPIC_FAN =
  "mec/cse08/industrial-monitor/fan";

// ---------- TIMING ----------

const unsigned long SENSOR_INTERVAL = 2000;
const unsigned long DEBOUNCE_DELAY = 20;
const unsigned long WIFI_RETRY_INTERVAL = 5000;
const unsigned long MQTT_RETRY_INTERVAL = 5000;

// ---------- OBJECTS ----------

DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient espClient;
PubSubClient mqtt(espClient);

// ---------- SHARED VARIABLES ----------

portMUX_TYPE dataMux = portMUX_INITIALIZER_UNLOCKED;

float temperature = 0;
float humidity = 0;
bool sensorReady = false;

bool fanState = false;
bool fanChanged = false;

// ---------- BUTTON VARIABLES ----------

int lastButtonReading = HIGH;
int stableButtonState = HIGH;

unsigned long lastDebounceTime = 0;

// ---------- SENSOR FUNCTION ----------

void readSensor() {

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("[DHT] Sensor reading failed!");
    return;
  }

  portENTER_CRITICAL(&dataMux);

  temperature = t;
  humidity = h;
  sensorReady = true;

  portEXIT_CRITICAL(&dataMux);

  Serial.println("----------------------");

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
}

// ---------- BUTTON FUNCTION ----------

void checkButton() {

  int reading = digitalRead(BUTTON_PIN);

  unsigned long now = millis();

  if (reading != lastButtonReading) {
    lastDebounceTime = now;
  }

  if (now - lastDebounceTime >= DEBOUNCE_DELAY) {

    if (reading != stableButtonState) {

      stableButtonState = reading;

      if (stableButtonState == LOW) {

        bool newState;

        portENTER_CRITICAL(&dataMux);

        fanState = !fanState;
        fanChanged = true;
        newState = fanState;

        portEXIT_CRITICAL(&dataMux);

        digitalWrite(LED_PIN, newState ? HIGH : LOW);

        Serial.print("[FAN] ");
        Serial.println(newState ? "ON" : "OFF");
      }
    }
  }

  lastButtonReading = reading;
}

// ---------- NETWORK TASK ----------

void networkTask(void* parameter) {

  unsigned long lastWiFiAttempt = 0;
  unsigned long lastMQTTAttempt = 0;
  unsigned long lastPublish = 0;

  bool firstWiFiAttempt = true;

  for (;;) {

    unsigned long now = millis();

    // WiFi connection management

    if (WiFi.status() != WL_CONNECTED) {

      if (mqtt.connected()) {
        mqtt.disconnect();
      }

      if (firstWiFiAttempt ||
          now - lastWiFiAttempt >= WIFI_RETRY_INTERVAL) {

        firstWiFiAttempt = false;
        lastWiFiAttempt = now;

        Serial.println("[WiFi] Connecting...");

        WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);
      }

      vTaskDelay(pdMS_TO_TICKS(20));
      continue;
    }

    // MQTT connection management

    if (!mqtt.connected()) {

      if (now - lastMQTTAttempt >= MQTT_RETRY_INTERVAL) {

        lastMQTTAttempt = now;

        String clientId =
          "MEC-ESP32-" +
          String((uint32_t)esp_random(), HEX);

        Serial.println("[MQTT] Connecting...");

        if (mqtt.connect(clientId.c_str())) {

          Serial.println("[MQTT] Connected!");

        } else {

          Serial.print("[MQTT] Failed, state: ");
          Serial.println(mqtt.state());
        }
      }

    } else {

      mqtt.loop();

      // Temperature and humidity publishing

      if (now - lastPublish >= SENSOR_INTERVAL) {

        lastPublish = now;

        float t, h;
        bool ready;

        portENTER_CRITICAL(&dataMux);

        t = temperature;
        h = humidity;
        ready = sensorReady;

        portEXIT_CRITICAL(&dataMux);

        if (ready) {

          char payload[100];

          snprintf(
            payload,
            sizeof(payload),
            "{\"temperature\":%.2f,\"humidity\":%.2f}",
            t, h
          );

          if (mqtt.publish(TOPIC_TEMP, payload)) {

            Serial.print("[MQTT] Published: ");
            Serial.println(payload);

          } else {

            Serial.println("[MQTT] Publish failed!");
          }
        }
      }

      // Fan status publishing

      bool changed;
      bool currentFanState;

      portENTER_CRITICAL(&dataMux);

      changed = fanChanged;
      currentFanState = fanState;

      if (changed) {
        fanChanged = false;
      }

      portEXIT_CRITICAL(&dataMux);

      if (changed) {

        if (!mqtt.publish(
              TOPIC_FAN,
              currentFanState ? "ON" : "OFF",
              true)) {

          Serial.println("[MQTT] Fan publish failed!");
        } else {

          Serial.println("[MQTT] Fan status published!");
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// ---------- SETUP ----------

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setSocketTimeout(2);
  mqtt.setKeepAlive(15);

  Serial.println("Industrial Monitoring System");
  Serial.println("System Starting...");

  xTaskCreatePinnedToCore(
    networkTask,
    "NetworkTask",
    8192,
    NULL,
    1,
    NULL,
    0
  );
}

// ---------- MAIN LOOP ----------

void loop() {

  static unsigned long lastSensorRead = 0;

  unsigned long now = millis();

  // Emergency button is checked continuously

  checkButton();

  // Read sensor every 2 seconds

  if (now - lastSensorRead >= SENSOR_INTERVAL) {

    lastSensorRead = now;

    readSensor();
  }
}
