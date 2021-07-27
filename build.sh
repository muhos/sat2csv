#!/bin/bash
cd src
g++ -o sat2csv sat2csv.cpp --std=c++11 -lstdc++fs
mkdir -p ../build
cp sat2csv ../build
