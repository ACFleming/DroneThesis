#!/bin/bash
cd ./build
echo "BUILDING"
cmake ..
cmake --build .
cd ..
echo "RUNNING"
echo "Log files will appear in logs dir"
./multiAgent

