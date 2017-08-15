#ifndef _Watchdog_H_
#define _Watchdog_H_

#include <ArduinoJson.h>
#include <EEPROM.h>
#include "Arduino.h"
#include <avr/wdt.h>

class Watchdog {
public:
    char* id;

    void resetNow() {
        asm volatile ( "jmp 0"); 
    }

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
        StaticJsonBuffer<300> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(json);

        if (root.success()) {

            if (root[F("cmd")].is<char*>()) {
                String cmd = F("reset");
                if (cmd.compareTo(root[F("cmd")].as<char*>()) == 0)
                    resetNow();
                return true;
            }
        }
        return false;
    }

};


#endif
