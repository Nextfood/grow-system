#ifndef _InfoTsl2591_H_
#define _InfoTsl2591_H_

#include "Arduino.h"
#include <ArduinoJson.h>
#include "SerialUtils.h"

class InfoTsl2591 {
public:
    char* id;
    tsl2591Gain_t gain;
    tsl2591IntegrationTime_t timing;

    void serializeJson() {
        StaticJsonBuffer<320> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root[F("id")] = id;
        JsonArray& devicesRoot = root.createNestedArray("devices");
        JsonObject& tsl2591Device = devicesRoot.createNestedObject();
        tsl2591Device[F("type")] = F("Light Sensor TSL2591");
        if (gain == TSL2591_GAIN_LOW) tsl2591Device[F("gain")] = F("LOW");
        else if (gain == TSL2591_GAIN_MED) tsl2591Device[F("gain")] = F("MED");
        else if (gain == TSL2591_GAIN_HIGH) tsl2591Device[F("gain")] = F("HIGH");
        else if (gain == TSL2591_GAIN_MAX) tsl2591Device[F("gain")] = F("MAX");

        if (timing == TSL2591_INTEGRATIONTIME_100MS) tsl2591Device[F("timing")] = F("100MS");
        else if (timing == TSL2591_INTEGRATIONTIME_200MS) tsl2591Device[F("timing")] = F("200MS");
        else if (timing == TSL2591_INTEGRATIONTIME_300MS) tsl2591Device[F("timing")] = F("300MS");
        else if (timing == TSL2591_INTEGRATIONTIME_400MS) tsl2591Device[F("timing")] = F("400MS");
        else if (timing == TSL2591_INTEGRATIONTIME_500MS) tsl2591Device[F("timing")] = F("500MS");
        else if (timing == TSL2591_INTEGRATIONTIME_600MS) tsl2591Device[F("timing")] = F("600MS");

        String jsonStr;
        root.printTo(jsonStr);
        SerialUtils::write(jsonStr);
    }

    bool deserializeJson(String& json) {
        StaticJsonBuffer<300> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(json);

        if (root.success()) {

            if (root[F("cmd")].is<char*>()) {
                String cmd = F("info");
                return (cmd.compareTo(root[F("cmd")].as<char*>())) == 0;
            }
        }
        return false;
    }
};


#endif
