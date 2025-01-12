#
#	CAN Tester for PEAK-System PCAN USB Interfaces (macOS, x86_64 & arm64)
#
#	Copyright (c) 2005-2010  Uwe Vogt, UV Software, Friedrichshafen
#	Copyright (c) 2012-2025  Uwe Vogt, UV Software, Berlin (info@uv-software.com)
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program   If not, see <https://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

TARGET  = can_test

INSTALL = ~/bin

PROJ_DIR = ../..
HOME_DIR = ..
MAIN_DIR = .

MISC_DIR = $(HOME_DIR)/misc
DRIVER_DIR = $(HOME_DIR)/driver
WRAPPER_DIR = $(HOME_DIR)/macOS

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/bitrates.o $(OUTDIR)/timer.o

DEFINES = 

HEADERS = -I$(MAIN_DIR) \
	-I$(HOME_DIR) \
	-I$(MISC_DIR)

ifeq ($(current_OS),Darwin)  # macOS - libPCBUSB.dylib

ifneq ($(VARIANT),STANDALONE)
  DEFINES += -DOPTION_PCBUSB_STANDALONE=0

  HEADERS += -I$(HOME_DIR)/macOS

  OBJECTS += $(OUTDIR)/PCBUSB.o

  BIN_DIR = $(HOME_DIR)/Binaries
else
  DEFINES += -DOPTION_PCBUSB_STANDALONE=1

  HEADERS += -I$(HOME_DIR)/macOS -I$(DRIVER_DIR)

  OBJECTS += $(OUTDIR)/pcan_api.o \
	$(OUTDIR)/pcan_drv.o \
	$(OUTDIR)/pcan_que.o \
	$(OUTDIR)/pcan_flt.o \
	$(OUTDIR)/pcan_trc.o \
	$(OUTDIR)/pcan_btr.o \
	$(OUTDIR)/pcan_log.o \
	$(OUTDIR)/pcan_usb.o \
	$(OUTDIR)/pcan_usbfd.o

  BIN_DIR = $(HOME_DIR)/../Binaries/$(TARGET)
endif

CFLAGS += -O2 -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O2 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS += -rpath /usr/local/lib

LIBRARIES = -lpthread

ifeq ($(VARIANT),STANDALONE)
  LDFLAGS += -Wl,-framework -Wl,IOKit -Wl,-framework -Wl,CoreFoundation
endif

ifeq ($(BINARY),UNIVERSAL)
CFLAGS += -arch arm64 -arch x86_64
CXXFLAGS += -arch arm64 -arch x86_64
LDFLAGS += -arch arm64 -arch x86_64
endif

CXX = clang++
CC = clang
LD = clang
endif

ifeq ($(current_OS),Linux)  # linux - libpcanbasic.so

DEFINES += 

HEADERS += -I$(HOME_DIR)/Linux

OBJECTS += $(OUTDIR)/PCBUSB.o

BIN_DIR = $(HOME_DIR)/Binaries

CFLAGS += -O2 -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O2 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS  +=

LIBRARIES = -lpthread

CXX = g++
CC = gcc
LD = gcc
endif

RM = rm -f
CP = cp -f

CHECKER  = warning,information
IGNORE   = -i tester.c -i dosopt.c

OUTDIR = .objects
BINDIR = $(BIN_DIR)

.PHONY: info outdir bindir


all: info outdir bindir $(TARGET)

info:
	@echo $(CC)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

bindir:
	@mkdir -p $(BINDIR)

incdir:
	@mkdir -p $(INCDIR)

check:
	cppcheck --enable=$(CHECKER) --suppressions-list=$(HOME_DIR)/suppress.txt \
	-D__APPLE__ $(DEFINES) $(HEADERS) $(IGNORE) $(MAIN_DIR) $(MISC_DIR) 

clean:
	@-$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	@-$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d
	@-$(RM) $(BINDIR)/$(TARGET)

install:
	@echo "Copying binary file..."
	@-$(CP) $(TARGET) $(INSTALL)


$(OUTDIR)/main.o: $(MAIN_DIR)/main.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/PCBUSB.o: $(WRAPPER_DIR)/PCBUSB.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/bitrates.o: $(MISC_DIR)/bitrates.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/timer.o: $(MISC_DIR)/timer.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_api.o: $(DRIVER_DIR)/pcan_api.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_drv.o: $(DRIVER_DIR)/pcan_drv.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_usb.o: $(DRIVER_DIR)/pcan_usb.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_usbfd.o: $(DRIVER_DIR)/pcan_usbfd.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_btr.o: $(DRIVER_DIR)/pcan_btr.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_flt.o: $(DRIVER_DIR)/pcan_flt.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_que.o: $(DRIVER_DIR)/pcan_que.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_trc.o: $(DRIVER_DIR)/pcan_trc.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/pcan_log.o: $(DRIVER_DIR)/pcan_log.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	$(CP) $(TARGET) $(BINDIR)
ifeq ($(current_OS),Darwin)
	@lipo -archs $@
endif
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
