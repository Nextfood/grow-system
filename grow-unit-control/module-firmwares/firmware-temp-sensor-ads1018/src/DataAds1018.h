#ifndef _DataAds1018_H_
#define _DataAds1018_H_

#include "Arduino.h"
#include <ArduinoJson.h>
#include "SerialUtils.h"

class DataAds1018 {
public:
    char* id;
    float temp = -273.15;
    float temp_internal = -273.15;
    unsigned int adc[2] = {0, 0};

    void serializeJson() {
        StaticJsonBuffer<320> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root[F("id")] = id;
        JsonObject& deviceObj = root.createNestedObject(F("devices"));
        JsonObject& sensor = deviceObj.createNestedObject(F("Temperature Sensor Ads1018"));
        sensor[F("temperature_internal")] = temp_internal;
        sensor[F("temperature")] = temp;

        JsonArray& tempAdcArray = sensor.createNestedArray(F("adc"));
        tempAdcArray.add(adc[0]);
        tempAdcArray.add(adc[1]);

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
