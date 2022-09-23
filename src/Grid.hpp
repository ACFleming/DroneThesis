#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <opencv2/opencv.hpp>
#include <set>
#include "Ring.hpp"
#include "BoundingPoints.hpp"

#define WAITKEY_DELAY 1

#define BASE "base_certainty"
#define MAP "map_of_grid"
#define LOCATIONS "locations_of_sources"



#define COST_VEC_PRINT
// #define SHOW_IMG


enum grid_vals{
    empty = 0,
    searching = 100,
    unknown = 200,
    occupied = 255
};


class Grid
{
private:

    static int field_x_width;
    static int field_y_length;

    std::pair<int,int> measurement_point;
    int measurement_range;
    std::vector<double> measurements;

    std::string name;
    Ring signal_ring;
    cv::Mat signal_likelihood;
    BoundingPoints signal_bounds;
    bool updated;
    bool found;

    





public:
    Grid();
    Grid(int field_x_width, int field_y_length, int value);
    Grid(std::string name, cv::Mat base_certainty);
    ~Grid();

    void markPoint(std::pair<int,int> point, int value);

    void prepareForUpdate(std::pair<int,int> point, int range);

    void receiveMeasurement( double measurement);

    void updateCertainty();

    std::vector<std::vector<cv::Point2i>>getImageFrontiers();

    static std::vector<std::vector<cv::Point2i>> getImageFrontiers(cv::Mat frontier_img);
    static cv::Mat rangeMask(std::pair<int,int> point,int radius, int value);

    bool isUpdated();
    bool isFound();

    BoundingPoints getSignalBounds();
    cv::Mat getLikelihood();


};

