#include "BoundingPoints.hpp"
BoundingPoints::BoundingPoints(){
    cv::Mat img  = cv::Mat::ones(10,10,CV_8UC1);
    this->bound = cv::boundingRect(img);
    int x_tl = this->bound.tl().x;
    int y_tl = this->bound.tl().y;
    int x_br = this->bound.br().x;
    int y_br = this->bound.br().y;


    this->top = cv::Point2i((x_tl+x_br)/2, y_tl);
    this->left = cv::Point2i(x_tl, (y_tl+y_br)/2);
    this->right = cv::Point2i(x_br, (y_tl+y_br)/2);
    this->bottom = cv::Point2i((x_tl+x_br)/2, y_br);
    this->centre = cv::Point2i((x_tl+x_br)/2, (y_tl+y_br)/2);
}

BoundingPoints::BoundingPoints(cv::Mat image) {
    this->bound = cv::boundingRect(image);
        
    int x_tl = this->bound.tl().x;
    int y_tl = this->bound.tl().y;
    int x_br = this->bound.br().x;
    int y_br = this->bound.br().y;


    this->top = cv::Point2i((x_tl+x_br)/2, y_tl);
    this->left = cv::Point2i(x_tl, (y_tl+y_br)/2);
    this->right = cv::Point2i(x_br, (y_tl+y_br)/2);
    this->bottom = cv::Point2i((x_tl+x_br)/2, y_br);
    this->centre = cv::Point2i((x_tl+x_br)/2, (y_tl+y_br)/2);
    
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
    return this->bound.area();
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