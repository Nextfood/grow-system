#!/usr/bin/python

import sys
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


def main():

    if len(sys.argv) < 4:
        print "Unable to start client with no serial device."
        print "Usage: client.py <serial device> <PWM> <ontime>"
        sys.exit(1)

    pwm = float(sys.argv[2])
    ontime = float(sys.argv[3])
    serial_port_device = SerialPortDevice(sys.argv[1])
    serial_port_device.open()
    time.sleep(2)

    try:

        dataJson = serial_port_device.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [0,0,0,0,0,pwm] }}}) + "\n")

        if not dataJson:
            raise Exception("Unable to get response from the water valve device.")
        message = json.loads(dataJson)
        if 'result' not in message and message['result'] != 'success':
            raise Exception("Unable to get successful result from the water valve device.")

        time.sleep(ontime)

        dataJson = serial_port_device.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [0,0,0,0,0,0] }}}) + "\n")

    except BaseException as e:
        print "Exception occurred. Try to stop: "  + str(e)
        serial_port_device.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [0,0,0,0,0,0] }}}) + "\n")


if __name__ == "__main__":
    main()
