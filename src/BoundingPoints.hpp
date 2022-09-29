#pragma once



#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <opencv2/opencv.hpp>



class BoundingPoints
{
private:
    cv::RotatedRect rect;
    cv::Point2i top;
    cv::Point2i left;
    cv::Point2i bottom;
    cv::Point2i right;
    cv::Point2i centre;

public:
    BoundingPoints();
    BoundingPoints(cv::Mat image);



    int getArea();
    cv::Point2i getTop();
    cv::Point2i getLeft();
    cv::Point2i getRight();
    cv::Point2i getBottom();
    cv::Point2i getCentre();
    std::vector<cv::Point2i> getAll();
    std::vector<cv::Point2i> getBounds();


};




// class BoundingPoints
// {
// private:
//     int x_tl = likelihood_boundary.tl().x;
//     int y_tl = likelihood_boundary.tl().y;
//     int x_br = likelihood_boundary.br().x;
//     int y_br = likelihood_boundary.br().y;

//     cv::Point2i top =       cv::Point2i((x_tl+x_br)/2, y_tl);
//     cv::Point2i left =      cv::Point2i(x_tl, (y_tl+y_br)/2);
//     cv::Point2i bottom =    cv::Point2i((x_tl+x_br)/2, y_br);
//     cv::Point2i right =     cv::Point2i(x_br, (y_tl+y_br)/2);
//     cv::Point2i centre =    cv::Point2i((x_tl+x_br)/2, (y_tl+y_br)/2);

// }