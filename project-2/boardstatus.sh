#!/bin/bash

path=$1
file1="stoc"
file2="pidfile.txt"
active=0
inactive=0
echo "$1"
for D in $1/*; do
	path1=$(printf '%s/%s' "$D" "$file1")
	path2=$(printf '%s/%s' "$D" "$file2")
	if [ -e $path1 ]  && [ -e $path2 ];
	then
		active=$((active+1))
	else
		inactive=$((inactive+1))
	fi
done
if [ $active -eq 1 ]
then
	echo "$active Board Active:"
else
	echo "$active Boards Active"
fi
for D in $1/*; do
	path1=$(printf '%s/%s' "$D" "$file1")
	path2=$(printf '%s/%s' "$D" "$file2")
	if [ -e $path1 ]  && [ -e $path2 ];
	then
		echo "$D"
	fi
done
if [ $inactive -eq 1 ]
then
	echo "$inactive inactive Board (on disk but no server running):"
else
	echo "$inactive inactive Boards (on disk but no server running):"
fi
for D in $1/*; do
	path1=$(printf '%s/%s' "$D" "$file1")
	path2=$(printf '%s/%s' "$D" "$file2")
	if [ ! -e $path1 ]  && [ ! -e $path2 ];
	then
		echo "$D"
	fi
done