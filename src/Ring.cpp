#include "Ring.hpp"

// cv::Mat Ring::intersectRings(std::vector<Ring> other_rings){
//     cv::Mat ret = cv::Mat::zeros(other_rings[0].y_length, other_rings[0].x_width, CV_8UC1)*255;
//     for(int i = 0; i  < other_rings.size(); i++){
//         other_rings[i].drawRing();
//         cv::bitwise_or(ret, other_rings[i].getCanvas(), ret);
//     }
//     return ret;
// }


// std::vector<std::pair<bool, cv::Point2i>> Ring::centroidOfIntersections(cv::Mat intersection){
//     std::vector<std::pair<bool, cv::Point2i>> centroids;
//     cv::Mat img = cv::Mat::zeros(intersection.size(),CV_8UC3);

//     std::vector<std::vector<cv::Point2i>> intersect_ctrs;
//     std::vector<cv::Vec4i> ctr_hierarchy;
//     cv::findContours(intersection,intersect_ctrs, ctr_hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);

//     for(int i = 0; i < intersect_ctrs.size(); i++){
//         cv::drawContours(img, intersect_ctrs, i, cv::Scalar(100, 255-30*(i+1), 30*i+150));
//         cv::imshow("img", img);
//         cv::waitKey(0);
//     }


//     for(auto &c: intersect_ctrs){
//         // cv::Rect r = cv::boundingRect(c);
//         // cv::Point2i p = cv::Point2i((r.br().x + r.tl().x)/2, (r.br().y+r.tl().y)/2);
//         // centroids.push_back(p);
//         cv::Moments M = cv::moments(c);
//         if(M.m00 > 0){
//             cv::Point2i p = cv::Point2i(M.m10/M.m00, M.m01/M.m00);
//             if(intersection.at<uint8_t>(p) > 0){
//                 centroids.push_back(std::make_pair(true,p));
//             }else{
//                 centroids.push_back(std::make_pair(false,p));
//             }
//         }

        
//     }
//     return centroids;
// }






Ring::Ring(int x_width, int y_length, int centre_x, int centre_y, double mean, double std_dev){
    this->x_width = x_width;
    this->y_length = y_length;
    this->centre_x = centre_x;
    this->centre_y = centre_y;
    this->mean = std::max(mean, 1.0);  
    this->std_dev = std_dev;
    this->canvas = cv::Mat::zeros(cv::Size(this->x_width, this->y_length), CV_8UC1);

    
}

Ring::Ring(){
    this->x_width = 1;
    this->y_length = 1;
    this->centre_x = 0;
    this->centre_y = 0;
    this->mean = 1;
    this->std_dev = 0;
    this->canvas = cv::Mat::zeros(cv::Size(this->x_width, this->y_length), CV_8UC1);
}

Ring::~Ring() {


}

void Ring::drawRing(){
    
    if(std_dev < 0){
        double scan_radius = this->mean;
        cv::circle(this->canvas, cv::Point2i(200,200), this->mean,  scan_radius,std_dev);
        cv::bitwise_not(this->canvas, this->canvas);
    }else{
        //change step size for more precision
        for(int c = 2; c >= 0; c = c-1){
            std::cout << 150*exp(-0.5*pow(c,2)) << std::endl;
            std::cout << 2*(c+1)*std_dev << std::endl;
            cv::circle(this->canvas, cv::Point2i(200,200), this->mean,  cv::Scalar(likely*exp(-0.5*pow(c,2))), 2*(c+1)*this->std_dev);
        }
    }

    




}

cv::Mat Ring::getCanvas(){
    return this->canvas; 
}




