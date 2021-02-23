#!/bin/bash

target_release() {
    cd release
    cmake -DCMAKE_BUILD_TYPE=Release ../..
    make
    echo "Built target in bin/release/"
    cd ../..
}

target_debug() {
    cd debug 
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make
    echo "Built target in bin/debug/"
    cd ../..
}

# Create folder for distribution
if [ "$1" = "release" ]
then
    if [ -d "$yare" ]
    then
        rm -rf -d yare
    fi

    mkdir -p yare
fi

# Creates the folder for the binaries
mkdir -p yare
mkdir -p bin
cd bin 
mkdir -p release
mkdir -p debug

# Builds target
if [ "$1" = "release" ]
then
    target_release
    cp bin/release/yare yare/yare
else
    target_debug
fi

cp -R Data yare/Data
