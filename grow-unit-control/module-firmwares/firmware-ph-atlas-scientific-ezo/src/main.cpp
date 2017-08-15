#include <Arduino.h>
#include <ArduinoJson.h>
#include "DeviceEzoPh.h"
#include "Id.h"
#include "SerialUtils.h"
#include "Watchdog.h"


#define BAUDRATE 57600
#define SERIALPORT_TIMEOUT_MS 1000
#define READ_BUFFER_SIZE 150

DataEzoPh g_data;
InfoEzoPh g_info;
DeviceEzoPh g_device;
ConfigureEzoPh g_conf;
Id g_id;
Watchdog g_watchdog;


#define RETURN_SUCCESS() \
{ \
    StaticJsonBuffer<320> jsonBuffer; \
    JsonObject& root = jsonBuffer.createObject(); \
    root[F("id")] = g_id.id; \
    root[F("result")] = F("success"); \
    String jsonStr; \
    root.printTo(jsonStr); \
    SerialUtils::write(jsonStr); \
    return; \
}

#define RETURN_FAILURE(errorMsg) \
{ \
    StaticJsonBuffer<320> jsonBuffer; \
    JsonObject& root = jsonBuffer.createObject(); \
    root[F("id")] = g_id.id; \
    root[F("result")] = F("failure"); \
    root[F("error_message")] = errorMsg; \
    String jsonStr; \
    root.printTo(jsonStr); \
    SerialUtils::write(jsonStr); \
    return; \
}

void parseCommand(String& json) {
  if (g_data.deserializeJson(json)) {
    g_data.serializeJson();
    return;
  } else if (g_info.deserializeJson(json)) {
    g_device.updateInfo(g_info).serializeJson();
    return;
  } else if (g_id.deserializeJson(json)) {
    g_id.serializeJson();
  } else if (g_watchdog.deserializeJson(json)) {
      g_watchdog.serializeJson();
      return;
  }

  if (g_conf.deserializeJson(json)) {
    g_device.configure(g_conf);
    g_conf.serializeJson();
  }

  RETURN_FAILURE(F("Unknown Command"));
}

void setup() {
  g_id.readIdFromEeprom();

  if (g_id.id[0] == 0 || g_id.id[0] == 0xff) {
    g_id.setIdWithoutEeprom(F("<unknown location>"));
  }

  g_data.id = g_id.id;
  g_info.id = g_id.id;
  g_conf.id = g_id.id;
  g_watchdog.id = g_id.id;

  Serial.setTimeout(SERIALPORT_TIMEOUT_MS);
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Setup devices
  SetupEzoPh s;
  s.id = g_id.id;
  g_device.setup(s);
  g_device.initiateUpdateData();
}

String readJson() {
    char buf[READ_BUFFER_SIZE];
    buf[0] = '\0';
    Serial.readBytesUntil('\n', buf, READ_BUFFER_SIZE);
    return String(buf);
}

void loop() {

  if (g_device.isUpdatedDataReady()) {
    g_device.updateData(g_data);
    g_device.initiateUpdateData();
  }

  String data = readJson();
  if (data.length()) {
    parseCommand(data);
  }

}
