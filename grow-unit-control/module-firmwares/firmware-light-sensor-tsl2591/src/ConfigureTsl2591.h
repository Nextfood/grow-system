#ifndef _ConfigureTsl2591_H_
#define _ConfigureTsl2591_H_

#include "Arduino.h"
#include <ArduinoJson.h>
#include "Adafruit_TSL2591.h"

class ConfigureTsl2591 {
public:
    tsl2591Gain_t gain;
    tsl2591IntegrationTime_t timing;

    bool deserializeJson(String& json) {
        StaticJsonBuffer<320> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(json);

        bool configured = false;
        if (root.success()) {
            if (root[F("devices")].is<JsonObject&>()) {
                JsonObject& devicesRoot = root[F("devices")].as<JsonObject&>();
                if (devicesRoot[F("Light Sensor TSL2591")].is<JsonObject&>()) {
                    JsonObject& tsl2591Root = devicesRoot[F("Light Sensor TSL2591")].as<JsonObject&>();
                    if (tsl2591Root[F("gain")].is<const char*>()) {
                        String v = tsl2591Root[F("gain")];
                        if (v.equalsIgnoreCase(F("LOW"))) {
                            gain = TSL2591_GAIN_LOW;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("MED"))) {
                            gain = TSL2591_GAIN_MED;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("HIGH"))) {
                            gain = TSL2591_GAIN_HIGH;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("MAX"))) {
                            gain = TSL2591_GAIN_MAX;
                            configured = true;
                        }
                    }
                    if (tsl2591Root[F("timing")].is<const char*>()) {
                        String v = tsl2591Root[F("timing")];
                        if (v.equalsIgnoreCase(F("100MS"))) {
                            timing = TSL2591_INTEGRATIONTIME_100MS;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("200MS"))) {
                            timing = TSL2591_INTEGRATIONTIME_200MS;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("300MS"))) {
                            timing = TSL2591_INTEGRATIONTIME_300MS;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("400MS"))) {
                            timing = TSL2591_INTEGRATIONTIME_400MS;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("500MS"))) {
                            timing = TSL2591_INTEGRATIONTIME_500MS;
                            configured = true;
                        } else if (v.equalsIgnoreCase(F("600MS"))) {
                            timing = TSL2591_INTEGRATIONTIME_600MS;
                            configured = true;
                        }
                    }
                }
            }
        }

        return configured;
    }
};


#endif
