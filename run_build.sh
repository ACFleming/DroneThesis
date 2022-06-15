#!/bin/bash
cd ./build
echo "BUILDING"
cmake ..
cmake --build .
cd ..
echo "Log files will appear in logs dir"

