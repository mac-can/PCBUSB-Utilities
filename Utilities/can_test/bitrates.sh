#!/bin/sh

Channel=PCAN-USB1

./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=20,nom_tseg1=12,nom_tseg2=3,nom_sjw=1,data_brp=4,data_tseg1=7,data_tseg2=2,data_sjw=1
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=255,nom_tseg2=64,nom_sjw=64,data_brp=2,data_tseg1=31,data_tseg2=8,data_sjw=8
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=127,nom_tseg2=32,nom_sjw=32,data_brp=2,data_tseg1=15,data_tseg2=4,data_sjw=4
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=63,nom_tseg2=16,nom_sjw=16,data_brp=2,data_tseg1=7,data_tseg2=2,data_sjw=2
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=31,nom_tseg2=8,nom_sjw=8,data_brp=2,data_tseg1=3,data_tseg2=1,data_sjw=1
./Debug/can_test $Channel --frames 0 --mode FDF --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=255,nom_tseg2=64,nom_sjw=64
./Debug/can_test $Channel --frames 0 --mode FDF --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=127,nom_tseg2=32,nom_sjw=32
./Debug/can_test $Channel --frames 0 --mode FDF --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=63,nom_tseg2=16,nom_sjw=16
./Debug/can_test $Channel --frames 0 --mode FDF --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=31,nom_tseg2=8,nom_sjw=8
echo
echo ~~~ CANopen FD bit-rates ~~~
echo
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=4,nom_tseg1=127,nom_tseg2=32,nom_sjw=32,data_brp=4,data_tseg1=6,data_tseg2=3,data_sjw=3
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=4,nom_tseg1=63,nom_tseg2=16,nom_sjw=16,data_brp=4,data_tseg1=6,data_tseg2=3,data_sjw=3
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=63,nom_tseg2=16,nom_sjw=16,data_brp=2,data_tseg1=14,data_tseg2=5,data_sjw=5
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_brp=2,nom_tseg1=31,nom_tseg2=8,nom_sjw=8,data_brp=2,data_tseg1=5,data_tseg2=2,data_sjw=2
echo
echo ~~~ UV Software ~~~
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_speed=125000,nom_sp=80.0,data_speed=1000000,data_sp=80.0
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_speed=250000,nom_sp=80.0,data_speed=2000000,data_sp=80.0
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_speed=500000,nom_sp=80.0,data_speed=4000000,data_sp=80.0
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_speed=1000000,nom_sp=80.0,data_speed=8000000,data_sp=80.0
./Debug/can_test $Channel --frames 0 --mode FDF+BRS --verbose --bitrate f_clock_mhz=80,nom_speed=2000000,nom_sp=80.0,data_speed=10000000,data_sp=80.0
