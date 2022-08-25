#include "Grid.hpp"


std::vector<std::vector<cv::Point2i>> Grid::getImageFrontiers(cv::Mat frontier_img) {
    std::vector<std::vector<cv::Point2i>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat approx_img = frontier_img.clone();


    cv::findContours(approx_img,contours, hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);
    

    if(contours.size() > 1){ //sorting by size. Could change this to check if the current point is inside or not, but may require point passing. 
        std::cout << contours.size();
        std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point2i> &a, const std::vector<cv::Point2i> &b){ return a.size() > b.size(); });
    }
  


    std::vector<std::vector<cv::Point2i>> ctr_approx;
    for(auto &e: contours){
        std::vector<cv::Point2i> a;
        cv::approxPolyDP(e, a, 0.75, true);
        ctr_approx.push_back(a);
    }

    for(int i = 0; i < ctr_approx.size(); i++){
        cv::drawContours(approx_img, ctr_approx, i, cv::Scalar(255,255,255));
    }

    
    cv::imshow("OCC approx map", approx_img);
    cv::waitKey(WAITKEY_DELAY);

  
    return ctr_approx;
}


int Grid::field_x_width;
int Grid::field_y_length;


cv::Mat Grid::rangeMask(std::pair<int,int> point, int radius, int value){
    cv::Mat mask = cv::Mat::zeros(Grid::field_y_length,Grid::field_x_width, CV_8UC1);
    cv::Point centre(point.first, point.second);
    cv::circle(mask, centre, radius, cv::Scalar(value,value,value), -1);
    return mask;
}


Grid::Grid(){
    this->name = "NULL";
}


Grid::Grid(int field_x_width, int field_y_length, int value){

    Grid::field_x_width = field_x_width;
    Grid::field_y_length = field_y_length;
    this->name = BASE;
    this->signal_ring = Ring();
    this->signal_likelihood = cv::Mat(Grid::field_y_length, Grid::field_x_width, CV_8UC1, cv::Scalar(unknown));
    this->signal_bounds = BoundingPoints();
    this->found = true;

}


Grid::Grid(std::string name, cv::Mat base_certainty){
    this->name = name;
    this->signal_ring = Ring();
    this->signal_likelihood = base_certainty.clone();
    this->signal_bounds = BoundingPoints();
    this->found = false;

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



void Grid::receiveMeasurement(double measurement) {
    
    std::cout << this->name << " @ dist " << measurement << std::endl;
    std::cout << "Measured from: " << this->measurement_point.first << "," << this->measurement_point.second << std::endl; 
    this->signal_ring = Ring(this->field_x_width, this->field_y_length, this->measurement_point.first, this->measurement_point.second,measurement, 20, name );
    this->signal_ring.drawRing();
    this->updated = true;


}


void Grid::updateCertainty(){

    // cv::imshow(this->name,this->signal_likelihood );
    cv::waitKey(WAITKEY_DELAY);
    if(!updated){ //i.e no new info  (Note: this should always trigger for base & map certainty grid)
        cv::Mat inv = Grid::rangeMask(this->measurement_point, this->measurement_range, 255);
        cv::bitwise_not(inv, inv);
        // cv::imshow("inv", inv);
        // cv::waitKey(WAITKEY_DELAY);
        cv::bitwise_and(inv, this->signal_likelihood,this->signal_likelihood);
    }else{
        // cv::imshow("new measurement", this->signal_ring.getCanvas());
        // cv::waitKey(WAITKEY_DELAY);
        cv::bitwise_and(this->signal_likelihood, this->signal_ring.getCanvas(), this->signal_likelihood);

    }
    // cv::imshow(this->name,this->signal_likelihood );
    cv::waitKey(WAITKEY_DELAY);

    if(this->found == false){ //dont need to calculate bounding for base or map
        BoundingPoints bp = BoundingPoints(this->signal_likelihood);
        if(bp.getArea() >= 50) this->signal_bounds = bp;

        if(this->signal_bounds.getArea() < 400){
            this->found = true;
            std::cout << "Signal found at or near: " << this->signal_bounds.getCentre().x  << "," << this->signal_bounds.getCentre().y << std::endl;
        }  
    }




    




}

std::vector<std::vector<cv::Point2i>> Grid::getImageFrontiers() {
    return Grid::getImageFrontiers(this->signal_likelihood);
}


bool Grid::isUpdated()  { return this->updated;   }
bool Grid::isFound()    { return this->found;     }

BoundingPoints Grid::getSignalBounds()  { return this->signal_bounds; }
cv::Mat Grid::getLikelihood()           { return this->signal_likelihood;}