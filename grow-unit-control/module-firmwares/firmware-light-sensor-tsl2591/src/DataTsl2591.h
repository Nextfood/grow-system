#ifndef _DataTsl2591_H_
#define _DataTsl2591_H_

#include "Arduino.h"
#include <ArduinoJson.h>
#include "SerialUtils.h"

class DataTsl2591 {
public:
    char* id;
    float full, ir, lux;
    bool error;
    String errorStr;

    void serializeJson() {
        StaticJsonBuffer<320> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root[F("id")] = id;
        JsonObject& deviceObj = root.createNestedObject(F("devices"));
        JsonObject& tsl2591Obj = deviceObj.createNestedObject(F("Light Sensor TSL2591"));
        tsl2591Obj[F("full")] = full;
        tsl2591Obj[F("ir")] = ir;
        tsl2591Obj[F("lux")] = lux;
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
