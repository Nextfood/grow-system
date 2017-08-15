#ifndef _ConfigureEzoPh_H_
#define _ConfigureEzoPh_H_

#include "Arduino.h"
#include <ArduinoJson.h>

class ConfigureEzoPh {
public:
  enum calPoint_t {
    CAL_LOWPOINT,
    CAL_MIDPOINT,
    CAL_HIGHPOINT
  };


  
  char* id;
  calPoint_t calPoint;
  float calTarget;

  



  void serializeJson() {
    StaticJsonBuffer<320> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root[F("id")] = id;
    root[F("result")] = F("success");
    String jsonStr;
    root.printTo(jsonStr);
    SerialUtils::write(jsonStr);
  }
    
  bool deserializeJson(String& json) {
    bool configured = false;
    StaticJsonBuffer<320> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (root.success()) {
      if (root[F("devices")].is<JsonObject&>()) {
        JsonObject& devicesRoot = root[F("devices")].as<JsonObject&>();
        if (devicesRoot[F("AS PH Sensor")].is<JsonObject&>()) {
          JsonObject& phSensorRoot = devicesRoot[F("AS PH Sensor")].as<JsonObject&>();
          if (phSensorRoot[F("cal_lowpoint")].is<float>()) {
            calPoint = CAL_LOWPOINT;
            String p = phSensorRoot[F("cal_lowpoint")];
            calTarget = p.toFloat();
            configured = true;
          } else if (phSensorRoot[F("cal_midpoint")].is<float>()) {
            calPoint = CAL_MIDPOINT;
            String p = phSensorRoot[F("cal_midpoint")];
            calTarget = p.toFloat();
            configured = true;
          } else if (phSensorRoot[F("cal_highpoint")].is<float>()) {
            calPoint = CAL_HIGHPOINT;
            String p = phSensorRoot[F("cal_highpoint")];
            calTarget = p.toFloat();
            configured = true;
          }
        }
      }
    }

    return configured;
  }

};


#endif
