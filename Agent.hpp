#pragma once

#include <vector>
#include "Coord.hpp"



//Class for the logic for the drone. This will not control the physical drone
class Agent
{
private:
    
    Coord curr_location; 
    std::vector<Coord> ally_locations;
    std::vector<Coord> known_signal_locations;
    

public:
    Agent(/* args */);
    ~Agent();
};




