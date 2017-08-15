resetusb:
	gcc -o resetusb resetusb.c

install: resetusb
	install -m 755 resetusb /usr/local/bin
	install -m 644 etc/udev/rules.d/90-nextfood-usbdev.rules /etc/udev/rules.d
	install -m 755 usr/local/bin/enumerate_nextfood_devices.py /usr/local/bin
        install -m 755 usr/local/bin/get_usb_device_number.sh /usr/local/bin
        install -m 755 usr/local/bin/resetAllUsbHubs.sh /usr/local/bin
        install -m 755 usr/local/bin/usblog_nextfood_devices.sh /usr/local/bin
