#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "GridValues.hpp"


class Ring
{
private:
    /* data */
    int x_width;
    int y_length;
    int centre_x;
    int centre_y;
    double middle_radius;   
    double ring_width;
    std::string name;


    cv::Mat canvas;

public:
    Ring(int x_width, int y_length, int centre_x, int centre_y, double middle_radius, double ring_width);
    Ring(int x_width, int y_length, int centre_x, int centre_y, double middle_radius, double ring_width, std::string name);
    ~Ring();

    void drawRing();
    cv::Mat getCanvas();
    std::string getName();

    static cv::Mat intersectRings(std::vector<Ring>other_rings);
    static std::vector<std::pair<bool, cv::Point2i>> centroidOfIntersections(cv::Mat intersection);
    static void dummy(){ return;}
};

