#pragma once
#include <iostream>
#include <stack>
#include <ctime>

#define PI 3.1415926535897

#define NO_OP [](){}()
#define WAITKEY_DELAY 1
#define BASE "base_certainty"
#define MAP "map_of_grid"
// #define LOCATIONS "locations_of_sources"

#define HERE std::cout << "HERE" << std::endl;



// #define COST_VEC_PRINT
// #define SHOW_IMG
// #define SHOW_MAP


#define DIST
#define SEEN
#define HULL_AREA
#define HULL_PERIM
#define INV_HULL_AREA
#define INV_HULL_PERIM
#define CHAIN

#define DOUBLE


#define TRIPLE





enum grid_vals{
    empty = 0,
    cleared = 50,
    searching = 100,
    likely = 200,
    occupied = 255
};



