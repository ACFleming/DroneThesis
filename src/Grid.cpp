#include "Grid.hpp"


std::vector<std::vector<cv::Point2i>> Grid::getImageFrontiers(cv::Mat frontier_img) {
    std::vector<std::vector<cv::Point2i>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat approx_img = frontier_img.clone();


    cv::findContours(approx_img,contours, hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);
    

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


Grid::Grid(std::string name, int field_x_width, int field_y_length, int value){

    Grid::field_x_width = field_x_width;
    Grid::field_y_length = field_y_length;
    this->name = name;
    this->signal_ring = Ring();
    this->signal_likelihood = cv::Mat(Grid::field_y_length, Grid::field_x_width, CV_8UC1, cv::Scalar(searching));
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
    this->signal_ring = Ring(this->field_x_width, this->field_y_length, this->measurement_point.first, this->measurement_point.second, measurement,std_dev);
    this->signal_ring.drawRing();
    this->updated = true;


}


void Grid::updateCertainty(){


    if(found) return;

    cv::Mat temp = this->signal_likelihood.clone();
// #ifdef SHOW_IMG
    cv::imshow(std::string(" Before"),this->signal_likelihood );
    cv::waitKey(WAITKEY_DELAY);
// #endif

    if(this->updated == false){ //i.e no new info  (Note: this should always trigger for base & map certainty grid)
        this->signal_ring = Ring(this->field_x_width, this->field_y_length, this->measurement_point.first, this->measurement_point.second, this->measurement_range,-1);
        
    }else{
        this->ping_counter++;
    }   
    // cv::imshow("new measurement", this->signal_ring.getCanvas());
    // cv::waitKey(WAITKEY_DELAY);
    this->signal_ring.drawRing();
    cv::bitwise_and(this->signal_likelihood, this->signal_ring.getCanvas(), temp);
// #ifdef SHOW_IMG
    cv::imshow(std::string(" result"), temp );
    cv::waitKey(WAITKEY_DELAY);
// #endif


    if(this->name != MAP){ //dont need to calculate bounding for map
        cv::Mat three_std_devs = temp.clone();
        cv::threshold(temp, three_std_devs, likely*exp(-0.5*pow(3,2))-1, 255, cv::THRESH_BINARY);
        cv::Mat one_std_dev = temp.clone();
        cv::threshold(temp, one_std_dev, likely*exp(-0.5*pow(1,2))-1, 255, cv::THRESH_BINARY);
// #ifdef SHOW_IMG
        cv::imshow(std::string(" 3 std desv"),three_std_devs );
        cv::waitKey(WAITKEY_DELAY);
// #endif
// #ifdef SHOW_IMG
        cv::imshow(std::string(" 1 std dev"),one_std_dev );
        cv::waitKey(WAITKEY_DELAY);
// #endif



        BoundingPoints three_std_confidence = BoundingPoints(three_std_devs);
        if(three_std_confidence.getArea() <= 300 || this->ping_counter > 5){
            this->found = true;
            this->signal_bounds = three_std_confidence;
            this->signal_likelihood = three_std_devs;

        }else{
            this->found = false;
            BoundingPoints one_std_confidence = BoundingPoints(one_std_dev);
            this->signal_bounds= one_std_confidence;
            this->signal_likelihood = one_std_dev;
            
        }
    }else{
        this->signal_likelihood = temp;
    }
// #ifdef SHOW_IMG
    cv::imshow(std::string(" After"),this->signal_likelihood );
    cv::waitKey(WAITKEY_DELAY);
// #endif




    




}

std::vector<std::vector<cv::Point2i>> Grid::getImageFrontiers() {
    return Grid::getImageFrontiers(this->signal_likelihood);
}


bool Grid::isUpdated()  { return this->updated;   }
bool Grid::isFound()    { return this->found;     }

BoundingPoints Grid::getSignalBounds()  { return this->signal_bounds; }
cv::Mat Grid::getLikelihood()           { return this->signal_likelihood;}