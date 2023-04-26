#!/bin/bash
set -u

dir="build"

if [ ! -d $dir ]; then
	mkdir -p $dir

	conan install . -if $dir -b missing -u

	cmake -S . -B $dir -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
fi

cmake --build $dir --config Release -j 4
