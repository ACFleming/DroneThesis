#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "Field.hpp"


enum occ_grid_vals{
    unknown,
    scanned,
    visited,
    occupied,
    frontier
};



class Agent
{
private:
    std::pair<int,int> coords;
    std::vector<std::pair<int,int>> coord_history;
   
    std::string name;
    int field_x_width;
    int field_y_length;
    int scan_radius;

    bool started_iter_grid;

    std::vector<std::vector<int>> occupancy_grid;


    // each pair is an id-measurement pair. Multiple of these pairs are measured each time step, collected in the inside vector.
    // the outside vector is a collection of all timesteps' measurements. 
    std::vector<std::vector<std::pair<std::string,double>>> measurements;

    int clipRange(int lower, int upper, int value);

    int *nextGridSquare();

    int markInRadius(occ_grid_vals value, bool justCount);



public:
    Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius);
    ~Agent();

    std::pair<int,int> determineAction();
    void moveToPosition(std::pair<int,int> pos) ;
    void measureSignalStrength(Field f);
    std::string logAgent();
    void showOccGrid();
};



