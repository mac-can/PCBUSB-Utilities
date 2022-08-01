### macOS&reg; Utilities for PCAN-USB Interfaces

*Copyright &copy;  2012-2022 by UV Software, Berlin.*\
*All rights reserved.*

# Running CAN and CAN FD on Mac&reg;

The PCBUSB library realizes a CAN to USB user-space driver under macOS for PCAN-USB interfaces from PEAK-System Technik, Darmstadt.
It supports up to 8 PCAN-USB and PCAN-USB FD devices.

The dynamic library libPCBUSB is running under macOS 10.13 and later (x86_64 architecture).
Since version 0.10.1 it is running under macOS Big Sur (11.x) or macOS Monterey (12.x) as Universal macOS Binary.  

See the [MacCAN](https://www.mac-can.com/) website to learn more.

## PCBUSB Utilities

This repo offers two command line tools that are build upon the PCBUSB library:
- `can_moni` to view incoming CAN messages
- `can_test` to test CAN communication

Both tools run as a standalone program, therefor it is not necessary to install the PCBUSB library to run them.

### can_moni

`can_moni` is a command line tool to view incoming CAN messages.
I hate this messing around with binary masks for identifier filtering.
So I wrote this little program to have an exclude list for single identifiers or identifier ranges (see program option `--exclude` or just `-x`).
Precede the list with a `~` and you get an include list.

Type `can_moni --help` to display all program options.

### can_test

`can_test` is a command line tool to test CAN communication.
Originally developed for electronic environmental tests on an embedded Linux system with SocketCAN, I´m using it for many years as a traffic generator for CAN stress-tests.

Type `can_test --help` to display all program options.

## Supported Devices

Only the following devices from PEAK-System Technik are supported:
- PCAN-USB (product code: IPEH-002021, IPEH-002022)

Since version 0.4 (Build 698 of September 22, 2017):
- PCAN-USB FD (product code: IPEH-004022)

## Known Bugs and Caveats

For a list of known bugs and caveats in the underlying PCBUSB library read the documentation of the appropriated library version.

PCAN-USB Pro FD devices are supported since version 0.10 of the PCBUSB library, but only the first channel (CAN1).

## This and That

Since version 0.5.3 macOS Big Sur (11.x) or macOS Monterey (12.x) is required to run the PCBUSB utilities as Universal macOS Binary.

This repo contains only binaries files of the PCBUSB utilities.

### License

The PCBUSB utilities are freeware without any warranty or support!

Please note the copyright and license agreement.

### Trademarks

Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other countries. \
PCAN is a registered trademark of PEAK-System Technik GmbH, Darmstadt, Germany. \
All other company, product and service names mentioned herein may be trademarks, registered trademarks or service marks of their respective owners.

### Hazard Note

_If you connect your CAN device to a real CAN network when using these utilities, you might damage your application._

### Contact

E-Mail: mailto://info@mac.can.com \
Internet: https://www.mac-can.com
