#!/usr/bin/python

import sys
import os.path
from os import listdir
from os.path import islink, join
import os
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


def reset_usb_device(device_name):
    device_number = subprocess.check_output(["/usr/local/bin/get_usb_device_number.sh", device_name])
    if device_number:
        call( ["/usr/local/bin/resetusb", device_number.split()[0], device_number.split()[1]] )


def main():
    if len(sys.argv) < 2:
        print "Unable to start client with no serial device."
        print "Usage: client.py <serial device>"
        sys.exit(1)

    serial_dev = sys.argv[1]

    devpath = "/tmp/dev/"


    # Cleaning up existing device
    onlyLinks = [f for f in listdir(devpath) if islink(join(devpath, f))]

    for links in onlyLinks:
        links = devpath + links
        if os.readlink(links) == serial_dev:
            call(["rm", "-f", links])

    # Creating device if the OS has not made the device after restarting the module
    if not os.path.exists(serial_dev):
        if not sys.argv[2] or not sys.argv[3]:
           print "Unable to make device {}. Missing Device Major/Minor numbers.".format(serial_dev)
           sys.exit(1)
        dev_major = sys.argv[2]
        dev_minor = sys.argv[3]
        call(["mknod", "--mode=0666", serial_dev, "c", dev_major, dev_minor])

    serial_port_device = SerialPortDevice(serial_dev)
    serial_port_device.open()
    time.sleep(2)

    dataJson = serial_port_device.serial_query(
        json.dumps({'cmd': 'info'}) + "\n")

    if not dataJson:
        print "Unable to acquire info from device {}. No response for Info call. Resetting device.".format(serial_dev)
        reset_usb_device(serial_dev)
        sys.exit(1)

    message = None
    try:
        message = json.loads(dataJson)
    except:
        print "Unable to acquire info from device {}. Unable to decode JSON return message. Resetting device.".format(serial_dev)
        reset_usb_device(serial_dev)
        sys.exit(1)

    id = message["id"]

    if not id:
        print "Unable to acquire info from device {}. ID was not found in the returned JSON. Resetting device.".format(serial_dev)
        reset_usb_device(serial_dev)
        sys.exit(1)

    call(["mkdir", "-p", devpath])
    call(["ln", "-sf", serial_dev, devpath + id])
    print "Symlink created: {} -> {}".format(id, serial_dev)

if __name__ == "__main__":
    main()
