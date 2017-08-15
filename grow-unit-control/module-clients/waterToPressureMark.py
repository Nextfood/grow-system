#!/usr/bin/python

import sys
import json
import serial
import serial.tools.list_ports
import time


max_pump_time = 20

class SerialPortDevice:

    _ser = None

    def __init__(self, serial_port):
        self._serial_port = serial_port

    def _readline(self):
        eol = b'\n'
        leneol = len(eol)
        line = bytearray()
        while True:
            c = self._ser.read(1)
            if c:
                line += c
                if line[-leneol:] == eol:
                    break
            else:
                break
        return bytes(line)

    def open(self):
        if self._ser:
            self._ser.close()
        self._ser = serial.Serial(self._serial_port, 57600, timeout=1)

    def serial_query(self, msg):
        self._ser.write(msg)
        self._ser.flush()
        read = self._readline()
        return read


def getWaterPressure(pres_sensor_device):
    dataJson = pres_sensor_device.serial_query(json.dumps({'cmd': 'data'}) + "\n")
    if not dataJson:
        raise Exception("Unable to get response from the water pressure sensor device from the last query.")

    message = json.loads(dataJson)
    return message['devices']['Pressure Sensor 1.2 MPa']['pressure']


class WaterPump:


    water_pump_running = False
    water_pump_device = None
    def __init__(self, water_pump_device):
        self.water_pump_device = water_pump_device

    def start(self):
        if not self.water_pump_running:
            self.water_pump_running = True
            dataJson = self.water_pump_device.serial_query(json.dumps({'devices': {'Relay x1': {"state": [True] }}}) + "\n")
            if not dataJson:
                raise Exception("Unable to get response from the device from the last query.")
            message = json.loads(dataJson)
            if 'result' not in message and message['result'] != 'success':
                raise Exception("Unable to get successful result from device from the last query.")


    def stop(self):
        if self.water_pump_running:
            dataJson = self.water_pump_device.serial_query(json.dumps({'devices': {'Relay x1': {"state": [False] }}}) + "\n")

            if not dataJson:
                raise Exception("Unable to get response from the device from the last query.")
            message = json.loads(dataJson)
            if 'result' not in message and message['result'] != 'success':
                raise Exception("Unable to get successful result from device from the last query.")
            self.water_pump_running = False


def main():

    if len(sys.argv) < 5:
        print "Unable to start client with no serial device."
        print "Usage: client.py <serial device water pump> <serial device pressure sensor> <pressure mark> <window size>"
        sys.exit(1)

    ser_dev_water_pump = sys.argv[1]
    ser_dev_pres_sensor = sys.argv[2]
    pressure_mark = float(sys.argv[3])
    pressure_window_size = float(sys.argv[4])

    water_pump_device = SerialPortDevice(ser_dev_water_pump)
    water_pump_device.open()

    pres_sensor_device = SerialPortDevice(ser_dev_pres_sensor)
    pres_sensor_device.open()

    time.sleep(2)

    try:
        wp = WaterPump(water_pump_device)

        current_pressure = getWaterPressure(pres_sensor_device)
        start_time = time.time()
        if current_pressure + pressure_window_size < pressure_mark:
            while(current_pressure < pressure_mark):
                wp.start()
#                print "Current pressure {} MPa".format(current_pressure)
                if (time.time() - start_time) > max_pump_time:
                    wp.stop()
                    raise Exception("Unable to reach water pressure of {} MPa after {} s. Current pressure: {} MPa."
                        .format(pressure_mark, max_pump_time, current_pressure))
                # Take average over few measurements. The pressure sensor swings when the pump starts.
                current_pressure = getWaterPressure(pres_sensor_device)
                current_pressure += getWaterPressure(pres_sensor_device)
                current_pressure += getWaterPressure(pres_sensor_device)
                current_pressure += getWaterPressure(pres_sensor_device)
                current_pressure /= 4
            wp.stop()
        print "Water pump run: {:.3f} s | Run pressure: {} MPa".format((time.time() - start_time),current_pressure)

    except BaseException as e:
        print "Exception occurred. Try to stop water pump: "  + str(e)
        water_pump_device.serial_query(json.dumps({'devices': {'Relay x1': {"state": [False] }}}) + "\n")
        sys.exit(1)

if __name__ == "__main__":
    main()

    
   
