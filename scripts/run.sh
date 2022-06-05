#!/bin/bash

if [ "$1" = "release" ]
then
    ./build/release/bin/rfrr 
else
    ./build/debug/bin/rfrr 
fi