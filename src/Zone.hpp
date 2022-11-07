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



class Zone
{
private:
    int x_cols;
    int y_rows;
    std::vector<cv::Point2i> edge_of_map;
    cv::Mat map_zone;
    /* data */
public:
    Zone(int x_cols, int y_rows);
    std::vector<cv::Point2i> getZoneEdges();
    bool inZone(cv::Point2i p);
};







