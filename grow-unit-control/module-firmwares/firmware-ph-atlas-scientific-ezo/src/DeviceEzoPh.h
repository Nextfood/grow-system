#ifndef _DeviceEzoPh_H_
#define _DeviceEzoPh_H_

#include <SPI.h>
#include <math.h>
#include "Arduino.h"
#include <SoftwareSerial.h>
#include "Device.h"
#include "SetupEzoPh.h"
#include "DataEzoPh.h"
#include "InfoEzoPh.h"
#include "ConfigureEzoPh.h"

#define DEVICE_RCV_BUFFER_SIZE 41

class DeviceEzoPh : public Device<SetupEzoPh, DataEzoPh, InfoEzoPh, ConfigureEzoPh> {
public:

  virtual void setup(SetupEzoPh& setup) {
    m_setup = setup;
    pinMode(setup.pin_rx, INPUT);
    pinMode(setup.pin_tx, OUTPUT);
    delay(1);
    _queryDeviceOk("C,0\r"); // Disable continous reading
  }

  virtual void initiateUpdateData() {
  }

  virtual bool isUpdatedDataReady() {
    return true;
  }

  virtual DataEzoPh& updateData(DataEzoPh& data) {
    String recv = _queryDevice("R\r");
    data.ph = recv.toFloat();
    return data;
  }

  virtual InfoEzoPh& updateInfo(InfoEzoPh& info) {
    return info;
  }

  virtual bool configure(ConfigureEzoPh& conf) {
    switch (conf.calPoint) {
      case ConfigureEzoPh::CAL_LOWPOINT:
        _queryDeviceOk(String("Cal,low,") + String(conf.calTarget, 2) + "\r");
        break;
      case ConfigureEzoPh::CAL_MIDPOINT:
        _queryDeviceOk(String("Cal,mid,") + String(conf.calTarget, 2) + "\r");
        break;
      case ConfigureEzoPh::CAL_HIGHPOINT:
        _queryDeviceOk(String("Cal,high,") + String(conf.calTarget, 2) + "\r");
        break;
      default:
        return false;
    };
    return true;
  }

private:

  String _readDevice(SoftwareSerial& serDev) {
    char buf[DEVICE_RCV_BUFFER_SIZE];
    buf[0] = '\0';
    size_t bytes = serDev.readBytesUntil('\r', buf, DEVICE_RCV_BUFFER_SIZE);
    if (bytes > 0) buf[bytes - 1] = '\0';
    return String(buf);
  }

  String _queryDevice(const String& msg) {
    SoftwareSerial serDev(m_setup.pin_rx, m_setup.pin_tx);
    serDev.begin(9600);
    serDev.setTimeout(1000);
    serDev.print(msg);

    return _readDevice(serDev);
  }

  bool _queryDeviceOk(const String& msg) {
    SoftwareSerial serDev(m_setup.pin_rx, m_setup.pin_tx);
    
    serDev.begin(9600);
    serDev.setTimeout(1000);
    serDev.print(msg);

    for (int i = 3; i; --i) {
      String recv = _readDevice(serDev);
      if (recv.startsWith("*OK"))
        return true;
    }
    return false;
  }

  SetupEzoPh m_setup;
};



#endif
