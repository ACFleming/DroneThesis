#!/bin/bash
cd ./build
echo "BUILDING with ${1:-DEBUG}"
cmake ..
cmake --build . --config "${1:-DEBUG}"
cd ..
echo "Log files will appear in logs dir"

