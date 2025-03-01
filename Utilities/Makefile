#
#	CAN Utilities for PEAK-System PCAN USB Interfaces (macOS, x86_64 & arm64)
#
#	Copyright (c) 2005-2010 Uwe Vogt, UV Software, Friedrichshafen
#	Copyright (c) 2012-2025 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
#
#	These utilities are free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	These utilities is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License along
#	with these utilities; if not, see <https://www.gnu.org/licenses/>.
#
all:
	@rm -r build_no.h 2> /dev/null || true
	@./build_no.sh
	@echo "\033[1mBuilding my beloved CAN Utilities...\033[0m"
	$(MAKE) -C can_test $@
	$(MAKE) -C can_moni $@
	$(MAKE) -C can_port $@

clean:
	$(MAKE) -C can_test $@
	$(MAKE) -C can_moni $@
	$(MAKE) -C can_port $@

pristine:
	$(MAKE) -C can_test $@
	$(MAKE) -C can_moni $@
	$(MAKE) -C can_port $@

install:
#	$(MAKE) -C can_test $@
#	$(MAKE) -C can_moni $@
#	$(MAKE) -C can_port $@

check:
	$(MAKE) -C can_test $@ 2> checker_test.txt
	$(MAKE) -C can_moni $@ 2> checker_moni.txt
	$(MAKE) -C can_port $@ 2> checker_port.txt

build_no:
	@./build_no.sh
	@cat ./build_no.h
