#!/bin/bash
set -u

dir="build"

if [ -d $dir ]; then
	cmake --build $dir --config Release -j 4
	exit 0
fi

mkdir -p $dir

cd $dir || exit

conan install .. -b missing -u

cmake -S .. -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES

cmake --build . --config Release -j 4

lnFile="compile_commands.json"

ln -sf $lnFile ..
