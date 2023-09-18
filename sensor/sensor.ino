#include <Wire.h>
#include <INA226_WE.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "config.h" // Include the configuration file

const unsigned long PUBLISH_INTERVAL = 3000;

INA226_WE ina226;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient1(ntpUDP, NTP_SERVER1, TIMEZONE_OFFSET * 3600, NTP_UPDATE_INTERVAL);

// Function prototypes
void initializeWiFi();
void initializeTime();
void initializeMQTT();
void initializeINA226();
void reconnectMQTT();
float readShuntVoltage();
float readLoadVoltage();
float readBusVoltage();
float readCurrent();
float readBusPower();
float readInternalTemperature();
void publishSensorData(unsigned long timestamp, float shuntVoltage, float busVoltage, float loadVoltage, float current, float power, float temperature);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  initializeWiFi();
  initializeTime();
  initializeMQTT();
  initializeINA226();
}

void loop() {
  timeClient1.update();
  unsigned long currentTimestamp = timeClient1.getEpochTime();

  float shuntVoltage_mV = readShuntVoltage();
  float loadVoltage_V = readLoadVoltage();
  float busVoltage_V = readBusVoltage();
  float current_mA = readCurrent();
  float power_mW = readBusPower();
  float internalTemperature_C = readInternalTemperature();

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  // Create and publish the JSON payload
  char payload[256];
  int payloadSize = snprintf(payload, sizeof(payload), "{\"device_id\":\"%s\",\"timestamp\":%lu,\"shunt_voltage_mV\":%.2f,\"bus_voltage_V\":%.2f,\"load_voltage_V\":%.2f,\"current_mA\":%.2f,\"bus_power_mW\":%.2f,\"internal_temperature_C\":%.2f}",
                             DEVICE_ID, currentTimestamp, shuntVoltage_mV, busVoltage_V, loadVoltage_V, current_mA, power_mW, internalTemperature_C);

  Serial.println("Publishing Payload:");
  Serial.println(payload);

  char mqttTopic[50];
  snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", MQTT_TOPIC, "data");

  if (mqttClient.connected()) {
    mqttClient.publish(mqttTopic, payload);
    Serial.println("Data published to MQTT broker");
  }

  delay(PUBLISH_INTERVAL);
}

// Function definitions
void initializeWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to WiFi");
}

void initializeTime() {
  timeClient1.begin();
  timeClient1.update();
  Serial.println("Time synchronized with NTP server");
}

void initializeMQTT() {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setClient(espClient);
  reconnectMQTT();
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect(DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker");
    } else {
      delay(5000);
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Trying again in 5 seconds...");
    }
  }
}

void initializeINA226() {
  ina226.init();
  Serial.println("INA226 initialized");
}

float readShuntVoltage() {
  ina226.readAndClearFlags();
  return ina226.getShuntVoltage_mV();
}

float readLoadVoltage() {
  float shuntVoltage_mV = readShuntVoltage();
  float busVoltage_V = ina226.getBusVoltage_V();
  return busVoltage_V + (shuntVoltage_mV / 1000);
}

float readBusVoltage() {
  return ina226.getBusVoltage_V();
}

float readCurrent() {
  return ina226.getCurrent_mA();
}

float readBusPower() {
  return ina226.getBusPower();
}

float readInternalTemperature() {
  return (float)temperatureRead();
}

void publishSensorData(unsigned long timestamp, float shuntVoltage, float busVoltage, float loadVoltage, float current, float power, float temperature) {
  DynamicJsonDocument jsonDoc(256);
  jsonDoc["device_id"] = DEVICE_ID;
  jsonDoc["timestamp"] = timestamp;
  jsonDoc["shunt_voltage_mV"] = shuntVoltage;
  jsonDoc["bus_voltage_V"] = busVoltage;
  jsonDoc["load_voltage_V"] = loadVoltage;
  jsonDoc["current_mA"] = current;
  jsonDoc["bus_power_mW"] = power;
  jsonDoc["internal_temperature_C"] = temperature;

  size_t payloadSize = measureJson(jsonDoc) + 1;
  char *payload = new char[payloadSize];
  serializeJson(jsonDoc, payload, payloadSize);

  char mqttTopic[50];
  snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", MQTT_TOPIC, "data");

  if (mqttClient.connected()) {
    mqttClient.publish(mqttTopic, payload);
    Serial.println("Data published to MQTT broker");
  }

  delete[] payload;
}
