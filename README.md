# DroneThesis
Repository for the development of my undergrad thesis: Radial signal source localisation with multi-agent exploration 

# Building
To build (requires cmake)
cd ./build
cmake ..
cmake --build .
then run ./multiAgent


OR ./build_and_run

OR g++ src/*.cpp -o main


FUTHER UPDATE

Using Eigen external library
"g++ -I ext_libs/ src/*.cpp -o main" will compile and run
Notes on how to change cmake is provided at https://eigen.tuxfamily.org/dox/TopicCMakeGuide.html 

