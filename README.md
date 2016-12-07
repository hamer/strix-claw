# strix-claw
This is a userspace daemon to prevent Strix Claw mouse from hanging when
special keys are pressed. It based mosly on Tristan Šneider's
[asus-claw.git](http://github.com/Tristaan/asus-claw.git).

## Building
A libusb-1.0 is required for building. On Debian Stretch it will be installed
by:

    # apt-get install libusb-1.0-0-dev

To build utility only make execution required:

    $ make

To run utility requires root's privileges:

    # ./strix-claw

## Installation
When no mouse connected, utility will exit immediately. This means, another
process should restart it periodically. This job can be done by systemd.

To install strix-claw to your system:

    # cp strix-claw /usr/local/bin
    # cp strix-claw.service /etc/systemd/system
    # systemctl enable strix-claw.service
    # systemctl start strix-claw.service

## TODO
 * support for sevaral simulatenous devices
 * implement a kernel module or patch usbhid subsystem
