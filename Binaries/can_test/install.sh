#!/bin/sh

ProjName=can_test
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
else
	echo "error"
fi