#include "Grid.hpp"




std::vector<std::vector<cv::Point2i>> Grid::getImageFrontiers(cv::Mat frontier_img) {
    std::vector<std::vector<cv::Point2i>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat approx_img = frontier_img.clone();


    cv::findContours(approx_img,contours, hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_NONE);
    

    if(contours.size() > 1){ //sorting by size. Could change this to check if the current point is inside or not, but may require point passing. 
        // std::cout << contours.size();
        std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point2i> &a, const std::vector<cv::Point2i> &b){ return a.size() > b.size(); });
    }
  
    


    std::vector<std::vector<cv::Point2i>> ctr_approx;
    for(auto &e: contours){
        std::vector<cv::Point2i> a;
        cv::approxPolyDP(e, a, 0.2, true);
        ctr_approx.push_back(a);
    }

    for(int i = 0; i < ctr_approx.size(); i++){
        cv::drawContours(approx_img, ctr_approx, i, cv::Scalar(255,255,255));
    }

#ifdef SHOW_IMG
    cv::imshow("OCC approx map", approx_img);
    cv::waitKey(WAITKEY_DELAY);
#endif

  
    return ctr_approx;
}


int Grid::field_x_rows;
int Grid::field_y_cols;


cv::Mat Grid::rangeMask(std::pair<int,int> point, int radius, int value){
    cv::Mat mask = cv::Mat::zeros(Grid::field_y_cols,Grid::field_x_rows, CV_8UC1);
    cv::Point centre(point.first, point.second);
    cv::circle(mask, centre, radius, cv::Scalar(value,value,value), -1);
    return mask;
}


Grid::Grid(){
    this->name = "NULL";
}


Grid::Grid(std::string name, int field_x_rows, int field_y_cols){

    Grid::field_x_rows = field_x_rows;
    Grid::field_y_cols = field_y_cols;
    this->name = name;
    this->signal_ring = Ring();
    this->signal_likelihood = cv::Mat(Grid::field_y_cols, Grid::field_x_rows, CV_8UC1, cv::Scalar(empty));
    
    // boundary.push_back(cv::Point2i(0.1*field_x_rows,0.1*field_y_cols));
    // boundary.push_back(cv::Point2i(0.3*field_x_rows,0.8*field_y_cols));
    // boundary.push_back(cv::Point2i(0.7*field_x_rows,0.8*field_y_cols));
    // boundary.push_back(cv::Point2i(0.9*field_x_rows,0.1*field_y_cols));


    this->edge_of_map.push_back(cv::Point2i(40,260));
    this->edge_of_map.push_back(cv::Point2i(140,280));
    // this->edge_of_map.push_back(cv::Point2i(240,260));
    // this->edge_of_map.push_back(cv::Point2i(280, 100));
    // this->edge_of_map.push_back(cv::Point2i(220, 20));
    this->edge_of_map.push_back(cv::Point2i(60,20));



    // for(int i = 0; i < 4; i++){
    //     cv::line(this->signal_likelihood, boundary[i],boundary[(i+1)%4], cv::Scalar(occupied));
    // }
    cv::fillConvexPoly(this->signal_likelihood, this->edge_of_map, cv::Scalar(searching));
    if(this->name == MAP){
        for(int i = 0; i < this->edge_of_map.size(); i++){
            cv::line(this->signal_likelihood, this->edge_of_map[i],this->edge_of_map[(i+1)%this->edge_of_map.size()], cv::Scalar(occupied));
        }
    }




    this->signal_bounds = BoundingPoints();
    this->found = false;
    this->ping_counter = 0;

}


Grid::Grid(std::string name, cv::Mat base_certainty){
    this->name = name;
    this->signal_ring = Ring();
    this->signal_likelihood = base_certainty.clone();
    this->signal_bounds = BoundingPoints();
    this->found = false;
    this->ping_counter = 0;

}

Grid::~Grid() {
    
}

void Grid::markPoint(std::pair<int,int> point, int value){
    this->signal_likelihood.at<uint8_t>(cv::Point2i(point.first, point.second)) = value;
}

void Grid::prepareForUpdate(std::pair<int, int> point, int range){
    this->signal_ring = Ring();
    this->updated = false;
    this->measurement_point = point; 
    this->measurement_range = range;
}



void Grid::receiveMeasurement(double measurement, double std_dev) {
    
    //NOTE ring with should be 6*std_dev
    
    // std::cout << this->name << " @ dist " << measurement << std::endl;
    // std::cout << "Measured from: " << this->measurement_point.first << "," << this->measurement_point.second << std::endl; 
    this->signal_ring = Ring(this->field_x_rows, this->field_y_cols, this->measurement_point.first, this->measurement_point.second, measurement,std_dev);
    this->signal_ring.drawRing();
    this->updated = true;


}


void Grid::updateCertainty(){

    if(this->name == MAP){
        return;


    }else if(this->name == BASE){
        cv::Mat cleared_zone = this->signal_likelihood.clone();
        cv::Mat valid_zone = this->signal_likelihood.clone();
        cv::threshold(valid_zone, valid_zone, 0, 255, cv::THRESH_BINARY);
#ifdef SHOW_IMG
        cv::imshow("Valid Zone", valid_zone);
        cv::waitKey(WAITKEY_DELAY);
#endif
        cv::circle(cleared_zone, cv::Point2i(this->measurement_point.first,this->measurement_point.second), this->measurement_range, cleared, -1 );
#ifdef SHOW_IMG
        cv::imshow("Cleared Zone", cleared_zone);
        cv::waitKey(WAITKEY_DELAY);
#endif
        // cv::subtract(this->signal_likelihood, cleared_zone, this->signal_likelihood);
        cv::bitwise_and(cleared_zone, valid_zone, this->signal_likelihood);
        // for(int i = 0; i < 50; i++){
        //     for(int j = 0; j < 50; j ++){
        //         std::cout << +(uint8_t)this->getLikelihood().at<uint8_t>(i,j) << " ";
        //     }
        //     std::cout << std::endl;
        // }
         



    }else{
        if(found) return;
            
            
        cv::Mat temp = this->signal_likelihood.clone();
#ifdef SHOW_IMG
    cv::imshow(std::string(" Before"),this->signal_likelihood );
    cv::waitKey(WAITKEY_DELAY);
#endif

        if(this->updated == false){ //i.e no new info  (Note: this should always trigger for base & map certainty grid)
            this->signal_ring = Ring(this->field_x_rows, this->field_y_cols, this->measurement_point.first, this->measurement_point.second, this->measurement_range,-1);
            
        }else{
            this->ping_counter++;
        }  
        // cv::imshow("new measurement", this->signal_ring.getCanvas());
        // cv::waitKey(WAITKEY_DELAY);
        this->signal_ring.drawRing();
        cv::bitwise_and(this->signal_likelihood, this->signal_ring.getCanvas(), temp);


#ifdef SHOW_IMG
        cv::imshow(std::string("ring & before"), temp );
        cv::waitKey(WAITKEY_DELAY);
#endif

        cv::Mat one_std_dev = temp.clone();
        cv::threshold(temp, one_std_dev, likely*exp(-0.5*pow(1,2))-1, 255, cv::THRESH_BINARY);
        BoundingPoints one_std_confidence = BoundingPoints(one_std_dev);


#ifdef SHOW_IMG
        cv::imshow(std::string(" 1 std dev"),one_std_dev );
        cv::waitKey(WAITKEY_DELAY);
#endif


        cv::Mat three_std_devs = temp.clone();
        cv::threshold(temp, three_std_devs, likely*exp(-0.5*pow(3,2))-1, 255, cv::THRESH_BINARY);
        BoundingPoints three_std_confidence = BoundingPoints(three_std_devs);
#ifdef SHOW_IMG
        cv::imshow(std::string(" 3 std desv"),three_std_devs );
        cv::waitKey(WAITKEY_DELAY);
#endif

        if(three_std_confidence.getArea() <= 300 || this->ping_counter > 5){
            this->found = true;
            this->signal_bounds = three_std_confidence;
            this->signal_likelihood = three_std_devs;

        }else{
            this->found = false;
            
            this->signal_bounds= one_std_confidence;
            this->signal_likelihood = one_std_dev;
            
        }






    
    }
    



    



 










#ifdef SHOW_IMG
    cv::imshow(std::string(" After"),this->signal_likelihood );
    cv::waitKey(WAITKEY_DELAY);
#endif




    




}

std::vector<std::vector<cv::Point2i>> Grid::getImageFrontiers() {
    return Grid::getImageFrontiers(this->signal_likelihood);
}


bool Grid::isUpdated()  { return this->updated;   }
bool Grid::isFound()    { return this->found;     }

BoundingPoints Grid::getSignalBounds()  { 
    return this->signal_bounds; 
}
cv::Mat Grid::getLikelihood()           { return this->signal_likelihood;}

std::vector<cv::Point2i> Grid::getMapEdges() {return this->edge_of_map;}
