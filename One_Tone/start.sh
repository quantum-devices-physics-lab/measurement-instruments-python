#!/bin/bash
version=$( python --version 2>&1)


if [[ $version =~ 2\.[0-9]+\.[0-9]+ ]]
then
	python3 One_Tone_GUI.py
elif [[ $version =~ 3\.[0-9]+\.[0-9]+ ]]
then
	python One_tone_GUI.py

else
	echo "problem detecting your python install or run manually\n Also make sure all the dependencies listed in the depedencies.txt file are installed in python 3"
fi
