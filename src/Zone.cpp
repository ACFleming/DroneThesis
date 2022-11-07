#include "Zone.hpp"


Zone::Zone(int x_cols, int y_rows){

    this->x_cols = x_cols;
    this->y_rows = y_rows; 


    // this->edge_of_map.push_back(cv::Point2i(0,0));
    // this->edge_of_map.push_back(cv::Point2i(0,y_rows-1));
    // this->edge_of_map.push_back(cv::Point2i(x_cols-1,y_rows-1));
    // this->edge_of_map.push_back(cv::Point2i(x_cols-1,0));



    // this->edge_of_map.push_back(cv::Point2i(0.1*x_cols,0.1*y_rows));
    // this->edge_of_map.push_back(cv::Point2i(0.3*x_cols,0.8*y_rows));
    // this->edge_of_map.push_back(cv::Point2i(0.7*x_cols,0.8*y_rows));
    // this->edge_of_map.push_back(cv::Point2i(0.9*x_cols,0.1*y_rows));


    // this->edge_of_map.push_back(cv::Point2i(0.1*field_x_cols,0.1*field_y_rows));
    // this->edge_of_map.push_back(cv::Point2i(0.1*field_x_cols,0.9*field_y_rows));
    // this->edge_of_map.push_back(cv::Point2i(0.9*field_x_cols,0.9*field_y_rows));
    // this->edge_of_map.push_back(cv::Point2i(0.9*field_x_cols,0.1*field_y_rows));




    this->edge_of_map.push_back(cv::Point2i(40,260));
    this->edge_of_map.push_back(cv::Point2i(140,280));
    this->edge_of_map.push_back(cv::Point2i(240,260));
    this->edge_of_map.push_back(cv::Point2i(280, 100));
    this->edge_of_map.push_back(cv::Point2i(220, 20));
    this->edge_of_map.push_back(cv::Point2i(60,20));

    // this->edge_of_map.push_back(cv::Point2i(60,120));
    // this->edge_of_map.push_back(cv::Point2i(60,260));
    // this->edge_of_map.push_back(cv::Point2i(240,260));
    // this->edge_of_map.push_back(cv::Point2i(160, 180));
    // this->edge_of_map.push_back(cv::Point2i(240, 120));
    // this->edge_of_map.push_back(cv::Point2i(60,20));

    this->map_zone = cv::Mat::zeros(y_rows, x_cols, CV_8UC1);
    cv::fillConvexPoly(this->map_zone, this->edge_of_map, 255);


}


std::vector<cv::Point2i> Zone::getZoneEdges(){
        return this->edge_of_map;
}


bool Zone::inZone(cv::Point2i p){
    
    cv::Mat mat = this->map_zone.clone();
    mat.at<uint8_t>(p) = 255;
    // cv::imshow("zone", mat);
    // cv::waitKey();
    uint8_t v = this->map_zone.at<uint8_t>(p);

    if(this->map_zone.at<uint8_t>(p) > 0){
        return true;
    }
    return false;
}

