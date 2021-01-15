#!/bin/bash
set -u

dir="build"

if [ -d $dir ]; then
    ninja -C $dir -j 4
    #cmake --build . --config Release
    exit 0
fi

mkdir -p $dir

cd $dir

conan install .. --build missing

cmake -S .. -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -DCMAKE_BUILD_TYPE=Release

ninja -C . -j 4
#cmake --build . --config Release

lnFile="compile_commands.json"

if [ ! -f $lnFile ]; then
    ln -s $dir/$lnFile .
fi
