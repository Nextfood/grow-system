#ifndef _DeviceTsl2591_H_
#define _DeviceTsl2591_H_

#include <avr/pgmspace.h>
#include "Device.h"
#include "SetupTsl2591.h"
#include "DataTsl2591.h"
#include "InfoTsl2591.h"
#include "ConfigureTsl2591.h"
#include "Adafruit_TSL2591.h"

class DeviceTsl2591 : public Device<SetupTsl2591,
DataTsl2591, InfoTsl2591, ConfigureTsl2591> {
public:

    virtual void setup(SetupTsl2591& setup) {
        if (!m_tsl.begin()) {
            m_error = true;
            m_errorStr = F("No TSL2591 sensor found. Check wiring.");
            return;
        }
        m_tsl.setGain(setup.gain);
        m_tsl.setTiming(setup.timing);
    }

    virtual void initiateUpdateData() {
        m_tsl.getFullLuminosityNonBlocking();
    }

    virtual bool isUpdatedDataReady() {
        return m_tsl.isFullLuminosityReady();
    }

    virtual DataTsl2591& updateData(DataTsl2591& data) {
        if (m_error == true) {
            data.error = true;
            data.errorStr = m_errorStr;
            return data;
        }
        uint32_t lum = m_tsl.getFullLuminosityNonBlockingData();
        uint16_t ir = lum >> 16;
        uint16_t full = lum & 0xFFFF;
        uint32_t lux = m_tsl.calculateLux(full, ir);
        data.full = full;
        data.ir = ir;
        data.lux = lux;
        data.error = false;
        return data;
    }

    virtual InfoTsl2591& updateInfo(InfoTsl2591& info) {
        info.gain = m_tsl.getGain();
        info.timing = m_tsl.getTiming();
        return info;
    }

    virtual bool configure(ConfigureTsl2591& conf) {
        m_tsl.setGain(conf.gain);
        m_tsl.setTiming(conf.timing);
        return true;
    }

private:


    bool m_error;
    String m_errorStr;


    Adafruit_TSL2591 m_tsl = Adafruit_TSL2591(0);
};



#endif
