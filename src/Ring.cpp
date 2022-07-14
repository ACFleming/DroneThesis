#include "Ring.hpp"

cv::Mat Ring::intersectRings(std::vector<Ring> other_rings){
    cv::Mat ret = cv::Mat::zeros(other_rings[0].y_length, other_rings[0].x_width, CV_8UC1)*255;
    for(int i = 0; i  < other_rings.size(); i++){
        other_rings[i].drawRing();
        cv::bitwise_or(ret, other_rings[i].getCanvas(), ret);
    }
    return ret;
}


std::vector<std::pair<bool, cv::Point2i>> Ring::centroidOfIntersections(cv::Mat intersection){
    std::vector<std::pair<bool, cv::Point2i>> centroids;
    cv::Mat img = cv::Mat::zeros(intersection.size(),CV_8UC3);

    std::vector<std::vector<cv::Point2i>> intersect_ctrs;
    std::vector<cv::Vec4i> ctr_hierarchy;
    cv::findContours(intersection,intersect_ctrs, ctr_hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);

    for(int i = 0; i < intersect_ctrs.size(); i++){
        cv::drawContours(img, intersect_ctrs, i, cv::Scalar(100, 255-30*(i+1), 30*i+150));
        cv::imshow("img", img);
        cv::waitKey(0);
    }


    for(auto &c: intersect_ctrs){
        // cv::Rect r = cv::boundingRect(c);
        // cv::Point2i p = cv::Point2i((r.br().x + r.tl().x)/2, (r.br().y+r.tl().y)/2);
        // centroids.push_back(p);
        cv::Moments M = cv::moments(c);
        if(M.m00 > 0){
            cv::Point2i p = cv::Point2i(M.m10/M.m00, M.m01/M.m00);
            if(intersection.at<uint8_t>(p) > 0){
                centroids.push_back(std::make_pair(true,p));
            }else{
                centroids.push_back(std::make_pair(false,p));
            }
        }

        
    }
    return centroids;
}



Ring::Ring(int x_width, int y_length, int centre_x, int centre_y, double middle_radius, double ring_width, std::string name){
    this->x_width = x_width;
    this->y_length = y_length;
    this->centre_x = centre_x;
    this->centre_y = centre_y;
    this->middle_radius = middle_radius;  
    this->ring_width = ring_width;
    this->canvas = cv::Mat::zeros(cv::Size(this->x_width, this->y_length), CV_8UC1);
    this->name = name;
    
}


Ring::Ring(int x_width, int y_length, int centre_x, int centre_y, double middle_radius, double ring_width){
    this->x_width = x_width;
    this->y_length = y_length;
    this->centre_x = centre_x;
    this->centre_y = centre_y;
    this->middle_radius = std::max(middle_radius, 1.0);  
    this->ring_width = ring_width;
    this->canvas = cv::Mat::zeros(cv::Size(this->x_width, this->y_length), CV_8UC1);
    this->name = "Dummy";

    
}

Ring::~Ring() {


}

void Ring::drawRing(){
    // cv::circle(this->canvas, cv::Point2i(this->centre_x, this->centre_y), this->middle_radius, cv::Scalar(255),0);
    int outer_radius =  std::max(this->middle_radius+0.5*this->ring_width, 1.0);
    int inner_radius =  std::max(this->middle_radius-0.5*this->ring_width, 0.0);
    //both outer and inner are at least 1;
    cv::circle(this->canvas, cv::Point2i(this->centre_x,this->centre_y),  outer_radius, cv::Scalar(target), -1);
    if(this->ring_width > 0 && inner_radius>=1){//if not asked to fill
        cv::circle(this->canvas, cv::Point2i(this->centre_x,this->centre_y), inner_radius, cv::Scalar(0), -1);

    }


}

cv::Mat Ring::getCanvas(){
    return this->canvas; 
}




std::string Ring::getName() {
    return this->name;
}
