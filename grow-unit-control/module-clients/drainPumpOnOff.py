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
        print "Usage: client.py <drain pump device> <water valve device> <drain time>"
        sys.exit(1)

    drain_pump_device = sys.argv[1]
    water_valves_device = sys.argv[2]
    ontime = float(sys.argv[3])

    drain_pump_dev = SerialPortDevice(drain_pump_device)
    drain_pump_dev.open()

    water_valves_dev = SerialPortDevice(water_valves_device)
    water_valves_dev.open()
    time.sleep(2)

    try:
        dataJson = water_valves_dev.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [0, 0, 0, 100, 0, 0]}}}) + "\n")
        if not dataJson:
            raise Exception("Unable to get response from the water valves device from the last query.")
        message = json.loads(dataJson)
        if 'result' not in message and message['result'] != 'success':
            raise Exception("Unable to get successful result from the water valves device from the last query.")



        dataJson = drain_pump_dev.serial_query(json.dumps({'devices': {'Relay x1': {"state": [True] }}}) + "\n")
        if not dataJson:
            raise Exception("Unable to get response from the water draining device from the last query.")
        message = json.loads(dataJson)
        if 'result' not in message and message['result'] != 'success':
            raise Exception("Unable to get successful result from the water draining device from the last query.")



        print "Ontime: {} s".format(ontime)
        time.sleep(ontime)



        dataJson = drain_pump_dev.serial_query(json.dumps({'devices': {'Relay x1': {"state": [False] }}}) + "\n")
        if not dataJson:
            raise Exception("Unable to get response from the water draining device when trying to shut it off.")
        message = json.loads(dataJson)
        if 'result' not in message and message['result'] != 'success':
            raise Exception("Unable to get successful result from the water draining device when trying to shut it off.")


        dataJson = water_valves_dev.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [0,0,0,0,0,0] }}}) + "\n")
        if not dataJson:
            raise Exception("Unable to get response from the water valves device when trying to shut it off.")
        message = json.loads(dataJson)
        if 'result' not in message and message['result'] != 'success':
            raise Exception("Unable to get successful result from the water valves device when trying to shut it off.")


    except BaseException as e:
        print "Exception occurred. Try to stop: "  + str(e)
        drain_pump_dev.serial_query(json.dumps({'devices': {'Relay x1': {"state": [False] }}}) + "\n")
        water_valves_dev.serial_query(json.dumps({'devices': {'PWM Power Driver x6': {"pwm": [0,0,0,0,0,0] }}}) + "\n")


if __name__ == "__main__":
    main()

    
   
