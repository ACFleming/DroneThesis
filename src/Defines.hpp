#pragma once

#define NO_OP [](){}()

#define WAITKEY_DELAY 1
// #define BASE "base_certainty"
#define MAP "map_of_grid"
#define LOCATIONS "locations_of_sources"

#define HERE std::cout << "HERE" << std::endl;



#define COST_VEC_PRINT
// #define SHOW_IMG


enum grid_vals{
    empty = 0,
    searching = 100,
    likely = 200,
};