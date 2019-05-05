#! /usr/bin/env bash
#TODO: get full path to apple script from $0, launch script 

if [ ! $# -eq 1 ]
then
	echo "Usage: launch.sh <program path>"
else
	echo $1 > /tmp/prog
	os=$(uname -s)
	if [ $os = "Darwin" ]
	then
		open -a Terminal.app $PWD/apple.sh
	fi
fi
#fi
#echo $prog $os
#echo $# # number of args passed in to this script
#/Users/brianlubeck/Documents/Programming/Projects/displayx/a.out
#sleep 4
