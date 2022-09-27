#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "Defines.hpp"



class Ring
{
private:
    /* data */
    int x_width;
    int y_length;
    int centre_x;
    int centre_y;
    double mean;   
    double std_dev;


    cv::Mat canvas;

public:
    Ring(int x_width, int y_length, int centre_x, int centre_y, double mean, double std_dev);
    Ring();
    ~Ring();

    void drawRing();
    cv::Mat getCanvas();

};

