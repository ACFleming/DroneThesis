#pragma once
#include <iostream>
#include <stack>
#include <ctime>

#define PI 3.1415926535

#define NO_OP [](){}()

#define WAITKEY_DELAY 1
#define BASE "base_certainty"
#define MAP "map_of_grid"
#define LOCATIONS "locations_of_sources"

#define HERE std::cout << "HERE" << std::endl;



// #define COST_VEC_PRINT
// #define SHOW_IMG


enum grid_vals{
    empty = 0,
    cleared = 75,
    searching = 125,
    likely = 200,
    occupied = 255
};



