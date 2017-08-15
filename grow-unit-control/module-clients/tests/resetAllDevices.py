#!/usr/bin/python

import sys
from subprocess import call
import json
import serial
import serial.tools.list_ports
import time

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

def find_serial_port():
    try:
        print "Finding serial port interface modules (on ID 1a86:7523)"
        print ""
        listPorts = serial.tools.list_ports.comports()
        call(["rm", "/tmp/dev/*"])
        for port in listPorts:
            if port.vid == 6790 and port.pid == 29987:
                try:
                    serial_port_device = SerialPortDevice(port.device)
                    serial_port_device.open()
                    time.sleep(2)
                    call(["/usr/local/bin/enumerate_nextfood_devices.py", port.device])

                except BaseException as e:
                    print "\nException occurred for device {}: ".format(port.device,str(e))
                    print ""
    except:
        print "Exception occurred when scanning for serial ports."


def main():
    serialport = find_serial_port()


if __name__ == "__main__": main()
