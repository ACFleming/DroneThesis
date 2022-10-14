#include "BoundingPoints.hpp"
BoundingPoints::BoundingPoints(){
    cv::Mat img  = cv::Mat::zeros(100,100,CV_8UC1);
    cv::line(img, cv::Point2i(20,20), cv::Point2i(20,70), cv::Scalar(255));
    cv::line(img, cv::Point2i(20,70), cv::Point2i(70,70), cv::Scalar(255));
    cv::line(img, cv::Point2i(70,70), cv::Point2i(70,20), cv::Scalar(255));
    cv::line(img, cv::Point2i(70,20), cv::Point2i(20,20), cv::Scalar(255));
    // cv::imshow("IMG", img);
    // cv::waitKey(0);
    std::vector<cv::Point2i> p;
    cv::findNonZero(img, p);
    this->rect = cv::minAreaRect(p);
    // this->bound = cv::boundingRect(image);
    cv::Point2f points[4];
    this->rect.points(points);
        


    this->bottom = points[0];
    this->left = points[1];
    this->top = points[2];
    this->right = points[3];
    this->centre = this->rect.center;
}

BoundingPoints::BoundingPoints(cv::Mat image) {
    assert(cv::countNonZero(image) > 0);
    std::vector<cv::Point2i> p;
    cv::findNonZero(image, p);
    this->rect = cv::minAreaRect(p);
    // this->bound = cv::boundingRect(image);
    cv::Point2f points[4];
    this->rect.points(points);
        


    this->bottom = points[0];
    this->left = points[1];
    this->top = points[2];
    this->right = points[3];
    this->centre = this->rect.center;

    
       /*
          
          
                    T
          TL _ _ _ _ _ _ _ _  
            |               |
            |               |
        L   |       C       |  R 
            |               |
            |               | 
            |_ _ _ _ _ _ _ _| BR

                    B 





        */

}






int BoundingPoints::getArea() {
    return this->rect.size.area();
}

cv::Point2i BoundingPoints::getTop() {
    return this->top;
}

cv::Point2i BoundingPoints::getLeft() {
    return this->left;
}

cv::Point2i BoundingPoints::getRight() {
    return this->right;
}

cv::Point2i BoundingPoints::getBottom() {
    return this->bottom;
}

cv::Point2i BoundingPoints::getCentre() {
    return this->centre;
}



std::vector<cv::Point2i> BoundingPoints::getAll() {
    std::vector<cv::Point2i> pts;
    pts.push_back(this->top);
    pts.push_back(this->left);
    pts.push_back(this->right);
    pts.push_back(this->bottom);
    pts.push_back(this->centre);
    return pts;
}

std::vector<cv::Point2i> BoundingPoints::getBounds() {
    std::vector<cv::Point2i> pts;
    pts.push_back(this->top);
    pts.push_back(this->left);
    pts.push_back(this->right);
    pts.push_back(this->bottom);
    return pts;
}

cv::RotatedRect BoundingPoints::getRotRect(){
    return this->rect;
}