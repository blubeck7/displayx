#! /usr/bin/env bash

# This script contains the Mac OS specific commands to open a new terminal
# window and launch the program inside of the new terminal window.

prog=$(</tmp/prog)
osascript -e\
	"tell app \"Terminal\" to set current settings of front window\
	to settings set \"scr\""
$prog
