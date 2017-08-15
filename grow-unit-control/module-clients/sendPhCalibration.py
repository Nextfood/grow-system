#!/usr/bin/python

import sys
import json
import serial
import serial.tools.list_ports
import time
import datetime




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
        self._ser = serial.Serial(self._serial_port, 57600, timeout=5)

    def serial_query(self, msg):
        self._ser.write(msg)
        self._ser.flush()
        read = self._readline()
        return read


def main():

    if len(sys.argv) < 3:
        print "Unable to start client with no serial device."
        print "Usage: client.py <serial device> <low|mid|high>"
        sys.exit(1)

    target_point = ''
    target_value = 0

    if sys.argv[2].lower() == "low":
       target_point = "cal_lowpoint"
       target_value = 4.00
    elif sys.argv[2].lower() == "mid":
       target_point = "cal_midpoint"
       target_value = 7.00
    elif sys.argv[2].lower() == "high":
       target_point = "cal_highpoint"
       target_value = 10.00
    else:
       print "Usage: client.py <serial device> <low|mid|high>"
       sys.exit(1)


    serial_port_device = SerialPortDevice(sys.argv[1])
    serial_port_device.open()
    time.sleep(2)
    try:
        msg = json.dumps({'devices': { 'AS PH Sensor' : {target_point: target_value } } }) + "\n"
        print "Sending: {}".format(msg)
        dataJson = serial_port_device.serial_query(msg)
        print dataJson
#        if not dataJson:
#            raise Exception("Unable to get response from device when trying to set the calibration point.")
#        message = json.loads(dataJson)
#        if 'result' not in message and message['result'] != 'success':
#            raise Exception("Unable to get successful response from device when trying to set the calibration point.")
    except BaseException as e:
        print "Exception occurred. Try to stop: "  + str(e)

if __name__ == "__main__":
    main()
