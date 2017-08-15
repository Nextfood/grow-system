#ifndef _DeviceAds1018_H_
#define _DeviceAds1018_H_

#include <SPI.h>
#include <math.h>
#include "Device.h"
#include "SetupAds1018.h"
#include "DataAds1018.h"
#include "InfoAds1018.h"
#include "ConfigureAds1018.h"

class DeviceAds1018 : public Device<SetupAds1018, DataAds1018, InfoAds1018, ConfigureAds1018> {
public:

    virtual void setup(SetupAds1018& setup) {
        m_lastUpdate = 0;
        m_setup = setup;
        pinMode(setup.pin_cs, OUTPUT);
        pinMode(setup.pin_sclk, OUTPUT);
        pinMode(setup.pin_mosi, OUTPUT);
        pinMode(setup.pin_miso, INPUT_PULLUP);

        digitalWrite(m_setup.pin_cs, HIGH);
        delay(1);
    }

    virtual void initiateUpdateData() {
    }

    virtual bool isUpdatedDataReady() {
        return true;
    }

    virtual DataAds1018& updateData(DataAds1018& data) {

        _readTemperature(data.temp, data.adc[0], data.adc[1], data.temp_internal);

        return data;
    }

    virtual InfoAds1018& updateInfo(InfoAds1018& info) {
        return info;
    }

    virtual bool configure(ConfigureAds1018& conf) {
        return false;
    }
private:

    float _externalVoltageRefCalculation(unsigned int adc, const SetupAds1018::ntc &ntc_setup) {
        float v = 0.0;
        if (adc == 0) {
            return v;
        } else if (adc >= 0x0800) {

            int intvalue = (0x7FF & (~adc)) + 1; // Calculate negative (2s complement)
            v = -(intvalue / 2048.0) * pgaToVoltage(ntc_setup.pga);
        } else {
            v = (adc / 2048.0) * pgaToVoltage(ntc_setup.pga);
        }
        return v;
    }

    float _externalTempCalculation(unsigned int adc, float refVoltage, const SetupAds1018::ntc &ntc_setup) {
        float res = 0.0;
        if (adc == 0) {
            return res;
        } else if (adc >= 0x0800) {

            int intvalue = (0x7FF & (~adc)) + 1; // Calculate negative (2s complement)
            res = -((intvalue/2048.0) * pgaToVoltage(ntc_setup.pga) * ntc_setup.refRes) / refVoltage;
        } else {
            res = ((adc/2048.0) * pgaToVoltage(ntc_setup.pga) * ntc_setup.refRes) / refVoltage;
        }

        float T = ntc_setup.b25_50 / log(res / (ntc_setup.res0 * exp(-ntc_setup.b25_50 / (273.15 + ntc_setup.t0))));

        return T - 273.15;
    }

    float _internalTempCalculation(unsigned int adc) {
        float res = 0.0;
        if (adc == 0) {
            return res;
        } else if (adc >= 0x0800) {

            int intvalue = (0x7FF & (~adc)) + 1; // Calculate negative (2s complement)
                    res = -(intvalue);
        } else {

            res = (adc);
        }
        return res * 0.125;
    }

    void _readTemperature(float &temp, unsigned int &adcCh1, unsigned int &adcCh2, float &tempInternal) {

        unsigned int adcTempInternal;
                _readAds1018(adcCh1, adcCh2, adcTempInternal);
                float refVoltage = _externalVoltageRefCalculation(adcCh1, m_setup.voltage_ref);
                temp = _externalTempCalculation(adcCh2, refVoltage, m_setup.thermistor);
                tempInternal = _internalTempCalculation(adcTempInternal);
    }

    unsigned int _rawReadAds1018(unsigned int conf) {
        digitalWrite(m_setup.pin_cs, LOW);
                delay(1);

                SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
                byte recv_msb = SPI.transfer((byte) (conf >> 8));
                byte recv_lsb = SPI.transfer((byte) (conf & 0x00FF));
                SPI.endTransaction();

                delay(1);
                digitalWrite(m_setup.pin_cs, HIGH);

        return (recv_msb << 4) | (recv_lsb >> 4);
    }

    void _readAds1018(unsigned int &adcCh1, unsigned int &adcCh2, unsigned int &adcIntTemp) {

        int conf;

                // Setup channel 1

                conf = 0x0000;
                // MSB
                conf |= (0x01 << 15); // SS
                conf |= ((int) m_setup.voltage_ref.mux << 12); // MUX
                conf |= ((int) m_setup.voltage_ref.pga << 9); // PGA
                conf |= (0x01 << 8); // MODE
                // LSB
                conf |= (m_setup.voltage_ref.datarate << 5); // DR
                conf |= (0x00 << 4); // TS_MODE
                conf |= (0x00 << 3); // PULL_UP_EN
                conf |= (0x01 << 1); // NOP

                _rawReadAds1018(conf); // Throw away results. Only loading config

                delay(10);

                // Setup channel 2

                conf = 0x0000;
                // MSB
                conf |= (0x01 << 15); // SS
                conf |= ((int) m_setup.thermistor.mux << 12); // MUX
                conf |= ((int) m_setup.thermistor.pga << 9); // PGA
                conf |= (0x01 << 8); // MODE
                // LSB
                conf |= (m_setup.thermistor.datarate << 5); // DR
                conf |= (0x00 << 4); // TS_MODE
                conf |= (0x00 << 3); // PULL_UP_EN        
                conf |= (0x01 << 1); // NOP

                adcCh1 = _rawReadAds1018(conf); // Results for channel 1

                delay(10);


                // Setup internal temp

                conf = 0x0000;
                // MSB
                conf |= (0x01 << 15); // SS
                conf |= ((int) m_setup.voltage_ref.mux << 12); // MUX
                conf |= ((int) m_setup.voltage_ref.pga << 9); // PGA
                conf |= (0x01 << 8); // MODE
                // LSB
                conf |= (m_setup.voltage_ref.datarate << 5); // DR
                conf |= (0x01 << 4); // TS_MODE
                conf |= (0x00 << 3); // PULL_UP_EN  
                conf |= (0x01 << 1); // NOP

                adcCh2 = _rawReadAds1018(conf); // Results for channel 2

                delay(10);


                // Irrelevant setup

                conf = 0x0000;
                // MSB
                conf |= (0x01 << 15); // SS
                conf |= ((int) m_setup.voltage_ref.mux << 12); // MUX
                conf |= ((int) m_setup.voltage_ref.pga << 9); // PGA
                conf |= (0x01 << 8); // MODE
                // LSB
                conf |= (m_setup.voltage_ref.datarate << 5); // DR
                conf |= (0x00 << 4); // TS_MODE
                conf |= (0x00 << 3); // PULL_UP_EN
                conf |= (0x01 << 1); // NOP

                adcIntTemp = _rawReadAds1018(conf); // Results for internal temp
                delay(10);


    }

    float pgaToVoltage(SetupAds1018::pgaEnum pga) {
        switch (pga) {
            case SetupAds1018::PGA_FSR_6_144V: return 6.144;
            case SetupAds1018::PGA_FSR_4_096V: return 4.096;
            case SetupAds1018::PGA_FSR_2_048V: return 2.048;
            case SetupAds1018::PGA_FSR_1_024V: return 1.024;
            case SetupAds1018::PGA_FSR_0_512V: return 0.512;
            case SetupAds1018::PGA_FSR_0_256V: return 0.256;
            default: return 0;
        };
    }

    unsigned long m_lastUpdate;
            SetupAds1018 m_setup;
};



#endif
