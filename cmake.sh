#!/bin/bash

set -u

folder="build"

if [ ! -d "$folder" ]; then
	mkdir -p $folder
fi

cmake -B$folder -H. -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES

ninja -C $folder -j 6

lnFile="compile_commands.json"

if [ ! -f "$lnFile" ]; then
	ln -s $folder/$lnFile .
fi
