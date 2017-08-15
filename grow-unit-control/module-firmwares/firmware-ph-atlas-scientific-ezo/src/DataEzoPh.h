#ifndef _DataEzoPh_H_
#define _DataEzoPh_H_

#include "Arduino.h"
#include <ArduinoJson.h>
#include "SerialUtils.h"

class DataEzoPh {
public:
  char* id;
  float ph = 0;

  void serializeJson() {
    StaticJsonBuffer<320> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root[F("id")] = id;
    JsonObject& deviceObj = root.createNestedObject(F("devices"));
    JsonObject& sensor = deviceObj.createNestedObject(F("AS PH Sensor"));
    sensor[F("ph")] = ph;

    String jsonStr;
    root.printTo(jsonStr);
    SerialUtils::write(jsonStr);
  }

  bool deserializeJson(String& json) {
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (root.success()) {
      if (root[F("cmd")].is<char*>()) {
        String cmd = F("data");
        return (cmd.compareTo(root[F("cmd")].as<char*>())) == 0;
      }
    }
    return false;
  }
};


#endif
