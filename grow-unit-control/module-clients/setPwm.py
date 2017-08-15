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


def activeDriver(dev, ch1,ch2,ch3,ch4,ch5,ch6):
        dataJson = dev.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [ch1,ch2,ch3,ch4,ch5,ch6] }}}) + "\n")

        if not dataJson:
            raise Exception("Unable to get response from the PWM device when trying to shut it off.")
        message = json.loads(dataJson)
        if 'result' not in message and message['result'] != 'success':
            raise Exception("Unable to get successful result from the PWM device when trying to set it to [{},{},{}]".format(ech1,ch2,ch3,ch4,ch5,ch6))


def usage():
    print "Usage: client.py <serial device> <on time|-1> <ch1,ch2,ch3,ch4,ch5,ch6>"
    sys.exit(1)

def main():

    if len(sys.argv) < 4:
        usage()

    on_time = float(sys.argv[2])
    ch_pcts = sys.argv[3].split(",")

    if len(ch_pcts) != 6:
        usage()

    serial_port_device = SerialPortDevice(sys.argv[1])
    serial_port_device.open()
    time.sleep(2)


    try:
        activeDriver(serial_port_device, ch_pcts[0],ch_pcts[1],ch_pcts[2],ch_pcts[3],ch_pcts[4],ch_pcts[5])

        if on_time > 0:
            time.sleep(on_time)
            activeDriver(serial_port_device, 0,0,0,0,0,0)


    except BaseException as e:
        print "Exception occurred. Trying to stop device: "  + str(e)
        serial_port_device.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [0,0,0,0,0,0] }}}) + "\n")


if __name__ == "__main__":
    main()
