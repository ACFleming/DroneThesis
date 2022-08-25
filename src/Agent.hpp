#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <functional>
#include <opencv2/opencv.hpp>
#include <set>
#include "Field.hpp"
#include "Ring.hpp"
#include "Grid.hpp"
#include "BoundingPoints.hpp"


#define PI 3.1415926535

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


    static int step_counter;



    std::vector<std::vector<cv::Point2i>> frontiers;
    std::map<std::string, Grid> *certainty_grids;





// should probably change these ints to double? the ints is nice for cartesian stuff

public:
    Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius, int speed, std::map<std::string, Grid> *certainty_grids);
    ~Agent();


    std::pair<int,int> determineAction();
    std::pair<int,int> moveToPosition(std::pair<int,int> pos) ;


    std::pair<int,int> point2Pair(cv::Point p);
    cv::Point pair2Point(std::pair<int,int> p);

    int clipRange(int lower, int upper, int value);
    double dist(std::pair<int,int> p1, std::pair<int,int> p2);


    cv::Mat rangeMask(int x, int y, int value);



    std::vector<std::vector<cv::Point2i>> getImageFrontiers(cv::Mat frontier_img);


    void updateCertainty(Field f); // this will be changed to read bluetooth signals

 
    void updateMap();
    



    int run();
    
    std::pair<int,int> getCoords();
    std::string logAgent();
    void showOccGrid();
    cv::Mat getMap();


    cv::Mat getSignalLocations();

};



