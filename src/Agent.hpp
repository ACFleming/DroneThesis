#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Field.hpp"
#include "Ring.hpp"
#include "GridValues.hpp"


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
    int bearing;

    int raster_step_count;

    cv::Mat certainty_grid;

    std::vector<std::vector<cv::Point2i>> frontiers;


    // each pair is an id-measurement pair. Multiple of these pairs are measured each time step, collected in the inside vector.
    // the outside vector is a collection of all timesteps' measurements. 
    std::vector<std::vector<std::pair<std::string,double>>> measurements;
    std::map<std::string, std::vector<Ring>> signal_estimations;
    std::map<std::string, cv::Mat > signal_locations;
    std::map<std::string, bool> is_found;





// should probably change these ints to double? the ints is nice for cartesian stuff

public:
    Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius, int speed, int bearing);
    ~Agent();


    std::pair<int,int> determineAction();
    void moveToPosition(std::pair<int,int> pos) ;


    std::pair<int,int> point2Pair(cv::Point p);
    cv::Point pair2Point(std::pair<int,int> p);

    int clipRange(int lower, int upper, int value);
    double dist(std::pair<int,int> p1, std::pair<int,int> p2);

    double angleInsideArc(std::pair<int,int> c, std::pair<int,int> p1, std::pair<int,int> p2);
    double angleWithHorizontal(std::pair<int,int>c, std::pair<int,int> p);

    cv::Mat rangeMask(int x, int y, int value);

    void drawArc(cv::Mat img, std::pair<int,int> c, std::pair<int,int> p1, std::pair<int,int> p2);


    std::vector<cv::Point2i> getOctagonPoints(int x, int y);


    std::vector<cv::Point2i> gridSquaresInRange(int x, int y);
    std::vector<cv::Point2i> gridSquaresInRange(std::pair<int,int> coords);
    std::vector<std::vector<cv::Point2i>> getImageFrontiers(cv::Mat frontier_img);


    std::pair<int,int> updateCertainty(Field f); // this will be changed to read bluetooth signals


    void updateScannedGrid();
    
    
    void measureSignalStrength(Field f);



    int run();
    
    std::pair<int,int> getCoords();
    std::string logAgent();
    void showOccGrid();
    cv::Mat getCertGrid();
    cv::Mat getFrontierMap();
};



