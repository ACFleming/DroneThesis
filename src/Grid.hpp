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







class Grid
{
protected:

    static int field_x_rows;
    static int field_y_cols;

    std::vector<cv::Point2i> edge_of_map;

    std::pair<int,int> measurement_point;
    int measurement_range;
    // std::vector<double> measurements;

    std::string name;
    Ring signal_ring;
    cv::Mat signal_likelihood;
    BoundingPoints signal_bounds;
    bool updated;
    bool found;
    int ping_counter;

    





public:
    Grid();
    Grid(std::string name, int field_x_rows, int field_y_cols);
    Grid(std::string name, cv::Mat base_certainty);
    ~Grid();

    void markPoint(std::pair<int,int> point, int value);

    void prepareForUpdate(std::pair<int,int> point, int range);

    void receiveMeasurement(double measurement, double std_dev);

    virtual void updateCertainty();

    std::vector<std::vector<cv::Point2i>>getImageFrontiers();

    static std::vector<std::vector<cv::Point2i>> getImageFrontiers(cv::Mat frontier_img);
    static cv::Mat rangeMask(std::pair<int,int> point,int radius, int value);

    bool isUpdated();
    bool isFound();

    BoundingPoints getSignalBounds();
    cv::Mat getLikelihood();
    cv::Mat getSignalLocation();

    std::vector<cv::Point2i> getMapEdges();


};

