#!/bin/bash
cd sat2csv
g++ -o sat2csv sat2csv.cpp --std=c++11 -lstdc++fs
mkdir -p ../build
cp sat2csv ../build
