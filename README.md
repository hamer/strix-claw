# strix-claw
This is an userspace utility for prevention of hanging Strix Claw mouse when DPI
or side buttons are pressed.  are pressed. It reads events from interface 2, which
is commonly used for additional features for mices and keyboards.

As an example, a part of `lsusb` and `lsusb -t` with Strix Tactic Pro and Strix Claw:

    Bus 001 Device 009: ID 195d:1016 Itron Technology iONE
    Bus 001 Device 008: ID 195d:2039 Itron Technology iONE

    |__ Port 10: Dev 7, If 0, Class=Hub, Driver=hub/4p, 480M
        |__ Port 1: Dev 8, If 1, Class=Human Interface Device, Driver=usbhid, 12M
        |__ Port 1: Dev 8, If 2, Class=Human Interface Device, Driver=usbhid, 12M
        |__ Port 1: Dev 8, If 0, Class=Human Interface Device, Driver=usbhid, 12M
        |__ Port 2: Dev 9, If 2, Class=Human Interface Device, Driver=, 12M
        |__ Port 2: Dev 9, If 0, Class=Human Interface Device, Driver=usbhid, 12M
        |__ Port 2: Dev 9, If 1, Class=Human Interface Device, Driver=usbhid, 12M

Notice, that kernel driver for interface 2 for mouse is absent. Tristan Šneider
made an investigation and found, that reading data from that interface is enough.

[asus-claw.git](http://github.com/Tristaan/asus-claw.git).

## Building
A libusb-1.0 is required for building. On Debian Stretch it will can installed
by:

    # apt-get install libusb-1.0-0-dev

Building the utility is simple as that:

    $ make

Utility requires root's privileges to run:

    # ./strix-claw

## Installation
When utility autostart and run-keeper (utility will be terminated when mouse is
being disconnected), systemd can do that job for you.

To install strix-claw to your system:

    # cp strix-claw /usr/local/bin
    # cp strix-claw.service /etc/systemd/system
    # systemctl enable strix-claw.service
    # systemctl start strix-claw.service

## TODO
 * support of multiple devices simulatenously
 * implement a kernel module or patch usbhid subsystem
