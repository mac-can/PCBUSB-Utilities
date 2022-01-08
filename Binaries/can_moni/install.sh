#!/bin/sh

ProjName=can_moni
BaseDir=$(dirname $0)
InstallDir=/usr/local/bin

if [ -f "$BaseDir/$ProjName" ]
then
	if [ -d "$InstallDir" ]
	then
		cp "$BaseDir/$ProjName" "$InstallDir/$ProjName"
	else
		mkdir "$InstallDir"; cp "$BaseDir/$ProjName" "$InstallDir/$ProjName"
	fi
	chmod 755 "$InstallDir/$ProjName"
else
	echo "error"
fi