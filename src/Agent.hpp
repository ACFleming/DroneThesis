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
#include "Defines.hpp"




struct point_hash {
    inline std::size_t operator()(const cv::Point2i & v) const {
        return v.x*31 + v.y;
    }
};


class Agent
{
private:
    std::pair<int,int> coords;
    std::map<std::pair<int,int>, int> command_history;
   
    std::string name;
    int field_x_rows;
    int field_y_cols;
    double scan_radius;
    double measurement_std_dev;
    double speed;

    
    



    std::vector<std::vector<cv::Point2i>> frontiers;
    std::map<std::string, Grid> *certainty_grids;





// should probably change these ints to double? the ints is nice for cartesian stuff

public:
    Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, double scan_radius, double measurement_std_dev, double speed, std::map<std::string, Grid> *certainty_grids);
    ~Agent();
    static int step_counter;

    void recordCommand(std::pair<int,int> p);
    int countCommand(std::pair<int,int> p);

    std::ostream *output;

    std::pair<int,int> determineAction();

    cv::Mat validateGrid(cv::Mat in);

    void costFunction(std::vector<cv::Point2i> points, std::unordered_set<cv::Point,point_hash> signal_frontiers, std::unordered_set<cv::Point,point_hash> hole_centres, cv::Point2i &best_point, double &best_score);

    std::pair<int,int> moveToPosition(std::pair<int,int> pos) ;


    std::pair<int,int> point2Pair(cv::Point p);
    cv::Point pair2Point(std::pair<int,int> p);

    int clipRange(int lower, int upper, int value);
    double dist(std::pair<int,int> p1, std::pair<int,int> p2);


    cv::Mat rangeMask(int x, int y, int value);



    // std::vector<std::vector<cv::Point2i>> getImageFrontiers(cv::Mat frontier_img);


    void updateCertainty(Field f); // this will be changed to read bluetooth signals

 
    // void updateBase();
    

    

    int run();
    
    std::pair<int,int> getCoords();
    void logAgent();
    void showOccGrid();
    cv::Mat getMap();



    cv::Mat getSignalLocations();



};



