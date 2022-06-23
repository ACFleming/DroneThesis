#!/bin/bash
echo "CLEAR LOG FILES"
cd logs
rm *.csv
echo "CLEARING EXE"
cd ..
rm main
rm Debug/main.*


