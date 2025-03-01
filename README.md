### macOS&reg; CAN Utilities for PEAK-System PCAN USB Interfaces

*Copyright &copy; 2005-2010 by Uwe Vogt, UV Software, Friedrichshafen.* \
*Copyright &copy; 2012-2025 by Uwe Vogt, UV Software, Berlin.*

![macOS Build](https://github.com/mac-can/PCBUSB-Utilities/actions/workflows/macos-build.yml/badge.svg)

# Running CAN and CAN FD on Mac&reg;

The PCBUSB library realizes a CAN to USB user-space driver under macOS for PCAN USB interfaces from PEAK-System Technik, Darmstadt.
It supports up to 8 PCAN-USB or PCAN-USB FD devices.

The dynamic library libPCBUSB is running under macOS 10.13 and later (Intel x86_64 architecture)
and since version 0.10.1 under macOS 11.0 and later as Universal macOS Binary (Intel x86_64 architecture and Apple arm64 architecture).  

See the [MacCAN](https://www.mac-can.com/) website to learn more.

## PCBUSB Utilities

This repo offers two CAN command line tools that are build upon the PCBUSB library:
- `can_moni` to view incoming CAN messages
- `can_test` to test CAN communication

_Note: These utilities can also be build and used on Linux with PEAK's chardev driver._

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

### Target Platform

- macOS 13.0 and later (Intel and Apple silicon)
- Debian GNU/Linux 12 (Linux Kernel 6.x)

### Development Environments

#### macOS Sequoia
- macOS Sequoia (15.3.1) on a Mac mini (M4 Pro, 2024)
- Apple clang version 16.0.0 (clang-1600.0.26.6)

#### macOS Ventura
- macOS Ventura (13.7.4) on a MacBook Pro (2019)
- Apple clang version 14.0.3 (clang-1403.0.22.14.1)

#### Debian 12.9 ("bookworm")
- Debian 6.1.128-1 (2025-02-07) x86_64 GNU/Linux
- gcc (Debian 12.2.0-14) 12.2.0

### Required Library

#### macOS
- libPCBUSB.x.y.dylib - Version 0.13 or later (Latest is Greatest!)

#### Linux
- libpcanbasic.so - PCAN Driver and Library for Linux, Version 8.20

### Supported Devices

Only the following devices from PEAK-System Technik are supported:
- PCAN-USB (product code: IPEH-002021, IPEH-002022)
- PCAN-USB FD (product code: IPEH-004022)
- PCAN-USB Pro FD (item no. IPEH-004061), but only the first channel (CAN1)

### Known Bugs and Caveats

- For a list of known bugs and caveats see tab [Issues](https://github.com/mac-can/PCBUSB-Utilities/issues) in the GitHub repo.
- For a list of known bugs and caveats in the underlying PCBUSB library read the documentation of the appropriated library version.

### Licenses

This repository contains two versions of the PCBUSB utilities that are licensed under different licences:

- **Standalone Version** (macOS only):
  This version of the utilities does not require the PCBUSB library to be installed.
  The utilities are offered in binary form only.
  The executables can be found in the folder `$(PROJROOT)/Binaries`.
- **Open-source Version** (macOS and Linux):
  This version links the PCBUSB library at runtime.
  The PCBUSB library itself is not part of this work.
  The source code of the utilities can be found in the folder `$(PROJROOT)/Utilities`.

#### Licence for the Standalone Version

This program is freeware without any warranty or support! \
Please note the copyright and license agreements.

#### Licence for the Open-Source Version

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, see <https://www.gnu.org/licenses/>.

Note:  The libPCBUSB is licensed under a freeware license without any
warranty or support.  The libPCBUSB is not part of this program.
It can be downloaded from <https://www.mac-can.com/>.

### Trademarks

Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other countries and regions. \
PCAN is a registered trademark of PEAK-System Technik GmbH, Darmstadt, Germany. \
POSIX is a registered trademark of the Institute of Electrical and Electronic Engineers, Inc. \
GNU C/C++ is a registered trademark of Free Software Foundation, Inc. \
Linux is a registered trademark of Linus Torvalds. \
All other company, product and service names mentioned herein may be trademarks, registered trademarks, or service marks of their respective owners.

### Hazard Note

_If you connect your CAN device to a real CAN network when using these utilities, you might damage your application._

### Contact

E-Mail: mailto://info@mac.can.com \
Internet: https://www.mac-can.com
