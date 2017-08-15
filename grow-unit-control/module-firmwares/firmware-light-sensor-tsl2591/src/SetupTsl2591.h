#ifndef _SetupTsl2591_H_
#define _SetupTsl2591_H_

#include "Adafruit_TSL2591.h"

class SetupTsl2591 {
public:
    char* id;
    const tsl2591Gain_t gain = TSL2591_GAIN_MED;
    const tsl2591IntegrationTime_t timing = TSL2591_INTEGRATIONTIME_600MS;
};


#endif
