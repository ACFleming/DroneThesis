#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "Field.hpp"
#include <opencv2/opencv.hpp>


enum occ_grid_vals{
    unknown = 0,
    visited = 20,
    frontier = 80,
    scanned = 160,
    blocked = 240
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
    int speed;
    int bearing;

    int raster_step_count;

    cv::Mat occupancy_grid;


    // each pair is an id-measurement pair. Multiple of these pairs are measured each time step, collected in the inside vector.
    // the outside vector is a collection of all timesteps' measurements. 
    std::vector<std::vector<std::pair<std::string,double>>> measurements;





// should probably change these ints to double? the ints is nice for cartesian stuff

public:
    Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius, int speed, int bearing);
    ~Agent();


    std::pair<int,int> determineAction();
    void moveToPosition(std::pair<int,int> pos) ;


    int clipRange(int lower, int upper, int value);

    cv::Mat rangeMask(cv::Mat image, int x, int y);


    std::vector<cv::Point2i> gridSquaresInRange(int x, int y);
    std::vector<cv::Point2i> gridSquaresInRange(std::pair<int,int> coords);
    std::vector<cv::Point2i> globalFrontiers();


    std::vector<cv::Point2i> updatedScannedGrid(int x, int y);
    
    
    void measureSignalStrength(Field f);
    
    std::pair<int,int> getCoords();
    std::string logAgent();
    void showOccGrid();
    cv::Mat getOccGrid();
};



