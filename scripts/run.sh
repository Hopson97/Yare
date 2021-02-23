#!/bin/bash


    if [ "$1" = "release" ]
    then
        ./bin/release/yare
    else 
        ./bin/debug/yare
    fi