#include "EspMQTTClient.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

#include "config.h"

long previousMillis;
DynamicJsonDocument payload(200);
DynamicJsonDocument commands(200);

int scanrate = 10000;
bool led = false;
String output;

// Make sure the config.h file is configured with the correct info
EspMQTTClient client(
  AP_SSID,    // Wifi SSID
  PASSWORD,   // Wifi password
  SERVER,     // MQTT Broker server
  USERNAME,   // MQTT Broker username
  KEY,        // MQTT Broker password
  CLIENT,     // MQTT Client name
  SERVERPORT  // MQTT port
);


void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, led);

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  // Optional functionalities of EspMQTTClient:
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage(TOPIC "DDEATH/" CLIENT, "{\"status\": \"offline\"}", true);

}

// This function is called when everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(TOPIC "DCMD/" CLIENT, [](const String & payloadString) {
    deserializeJson(commands, payloadString);
    if (commands.containsKey("scanrate")) {
      scanrate = commands["scanrate"];
    }
    if (commands.containsKey("led")) {
      led = commands["led"];
    }
  });

  client.publish(TOPIC "DBIRTH/" CLIENT, "{\"status\": \"online\"}");
}


void loop() {
  digitalWrite(2, led);
  client.loop();
  checkSensors();
  yield();
}

void checkSensors() {
  bool timeToCheck = (millis() - previousMillis) > scanrate;
  if (timeToCheck) {
    if (! bme.performReading()) {
      Serial.println("Failed to perform reading :(");
      return;
    }
    payload["pressure"] = bme.pressure / 100.0;
    payload["gasResistance"] = bme.gas_resistance / 1000.0;
    payload["temperature"] = bme.temperature;
    payload["humidity"] = bme.humidity;
    String payloadString;
    serializeJson(payload, payloadString);
    client.publish(TOPIC "DDATA/" CLIENT, payloadString);
    previousMillis = millis();
  }
}
