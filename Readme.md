# ESP32 BME688 Example code
## Arduino IDE

Currently using Arduino IDE 1.8.13
## Libraries

All libraries can be installed directly from the Library Manager in the IDE
* EspMQTTClient 1.13.3 (will install PubSubClient)
* ArduinoJson 6.18.5
* Adafruit BME680 2.0.1 (will install Adafruit Unified Sensor Library)

## Configuration
1. Copy the `configexample.h` file and rename it `config.h`
1. Edit the settings in `config.h` for your ClientId (deviceId), mqtt broker and wifi settings

## Payload
Chispas protocol parser (lite version of Sparkplug B)

Topic format: `<Namespace>/<GroupId>/<MessageType>/<deviceId>`

Message types:
* DBIRTH (sent when device connects)
* DDEATH (sent by MQTT broker when device disconnects)
* DDATA (JSON payload)
* DDCMD (send commands to the device)

In my NODE-RED setup and INFLUX database I do the following mapping:
* The Measurement is taken from `<Namespace>/<GroupId>/<MessageType>`
* Incoming payload can be any single object (no nested objects or lists)
* The payload is mapped to the key/value fields in the database
* The deviceId from the topic is mapped to the Influx tags (indexes)