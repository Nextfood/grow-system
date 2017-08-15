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



def set_serialport_id(serialport, idstring):
    ser = SerialPortDevice(serialport)
    ser.open()

    time.sleep(2)
    
    dataJson = ser.serial_query(json.dumps({'id': idstring}) + "\n")
    if not dataJson:
        raise Exception("Unable to get response from the device from the last query.")
    message = json.loads(dataJson)
    if 'result' not in message and message['result'] != 'success':
        raise Exception("Unable to get successful result from device from the last query.")


def main():
    
    if len(sys.argv) < 3:
        print "Unable to start client with no serial port or identification string."
        print "Usage: client.py <serialport> <ID string>"
        sys.exit(1)

    set_serialport_id(sys.argv[1], sys.argv[2])
      
    

if __name__ == "__main__": main()



    
    