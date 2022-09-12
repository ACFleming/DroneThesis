#!/bin/bash
echo "CLEARING ${1} LOG FILES"
cd "${1}_agent_logs"
rm *.csv
echo "CLEARING ${1} IMAGE FILES"
cd ..
cd "${1}_agent_images"
rm *.png
echo "CLEARING EXE"
cd ..



