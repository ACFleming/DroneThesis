#include "Agent.hpp"


/*

    matricies work in row, col referencing.
    * - - - - - > cols
    |
    |
    |
    |
    |
    v
    rows

    open cv points work x,y but top lef origin

    * - - - - - > x
    |
    |
    |
    |
    |
    v
    y
    */

Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius, int speed, int bearing) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;
    this->field_x_width = field_width;
    this->field_y_length = field_length;
    this->scan_radius = scan_radius;
    this->speed = speed;
    this->bearing = bearing;
    this->raster_step_direction = -1;
    
    


    this->occupancy_grid = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC1);

        
    // // std::cout << this->occupancy_grid << std::endl;

    // cv::Rect boundary(0,0, this->field_x_width-1, this->field_y_length-1);

    // cv::rectangle(this->occupancy_grid, boundary, cv::Scalar(blocked, blocked, blocked));


    std::cout << this->occupancy_grid.depth() << std::endl;

    this->occupancy_grid.at<uint8_t>(this->coords.second,this->coords.first) = blocked;




    



}

Agent::~Agent() {


}




//clips value in range such that lower <= value < upper
int Agent::clipRange(int lower, int upper, int value){
    if(value < lower){
        value = lower;
    }else if(value >= upper){
        value = upper - 1;
    }
    return value;
}



cv::Mat Agent::rangeMask(int x, int y, int value){
    cv::Mat mask = cv::Mat::zeros(this->field_y_length,this->field_x_width, CV_8UC1);
    cv::Point centre(x,y);
    cv::circle(mask, centre, this->scan_radius, cv::Scalar(value,value,value), -1);
    return mask;
}


std::vector<cv::Point2i> Agent::getOctagonPoints(int x, int y){
    std::vector<cv::Point2i> clockwise_points;      
    cv::Point2i p;
    //N
    p = cv::Point( this->clipRange(0, this->field_x_width, int(x))                                ,this->clipRange(0, this->field_y_length, int(y - this->scan_radius)));
    clockwise_points.push_back(p);
    //NE
    // p = cv::Point(this->clipRange(0, this->field_x_width, int(x + sin(PI/4)*this->scan_radius))  ,this->clipRange(0, this->field_y_length, int(y - sin(PI/4)*this->scan_radius)));
    // clockwise_points.push_back(p);
    //E
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x + this->scan_radius))             ,this->clipRange(0, this->field_y_length, int(y)));
    clockwise_points.push_back(p);
    //SE
    // p = cv::Point(this->clipRange(0, this->field_x_width, int(x + sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y + sin(PI/4)*this->scan_radius)));
    // clockwise_points.push_back(p);
    //S
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x))                                 ,this->clipRange(0, this->field_y_length, int(y + this->scan_radius)));
    clockwise_points.push_back(p);
    //SW
    // p = cv::Point(this->clipRange(0, this->field_x_width, int(x - sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y + sin(PI/4)*this->scan_radius)));
    // clockwise_points.push_back(p);
    //W
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x - this->scan_radius))             ,this->clipRange(0, this->field_y_length, int(y)));
    clockwise_points.push_back(p);
    //NW
    // p = cv::Point(this->clipRange(0, this->field_x_width, int(x - sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y - sin(PI/4)*this->scan_radius)));
    // clockwise_points.push_back(p);
    return clockwise_points;

}

std::vector<cv::Point2i> Agent::gridSquaresInRange(int x, int y){

    std::vector<cv::Point2i> locations;

    cv::Mat mask = this->rangeMask(x, y, 255);


    cv::findNonZero(mask, locations);

    return locations;

}

std::vector<cv::Point2i> Agent::gridSquaresInRange(std::pair<int,int> coords){
    return this->gridSquaresInRange(coords.first, coords.second);
}

void Agent::updateScannedGrid(){

    cv::Mat mask = this->rangeMask(this->coords.first, this->coords.second, scanned);

    cv::bitwise_or(this->occupancy_grid, mask, this->occupancy_grid);



    // this->frontiers = this->getNewFrontiers(this->coords.first, this->coords.second);
    // std::cout << "Frontier count: " << this->frontiers.size() << std::endl; 




}

std::vector<std::vector<cv::Point2i>> Agent::getNewFrontiers(int x, int y){
    // std::vector<cv::Point2i> oct = this->getOctagonPoints(x, y);



    // cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);
    
    // std::vector<std::vector<cv::Point2i>> new_frontiers;
    // new_frontiers.push_back(oct);
    // // if(!this->frontiers.empty()) new_frontiers.push_back(this->frontiers);
    // for(auto &f: this->frontiers){
    //     new_frontiers.push_back(f);
    // }

    
    // cv::drawContours(frontier_map, new_frontiers, -1, cv::Scalar(255));
    
    // // cv::imshow("Frontier map", frontier_map);
    // // cv::waitKey(0);


    
    // std::vector<std::vector<cv::Point2i>> found_contours;
    // std::vector<cv::Vec4i> hierarchy;
    // cv::findContours(frontier_map,found_contours, hierarchy, cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);

    // //filter out non-external contours

    // // std::vector<std::vector<cv::Point2i>> ext_contours;

    // // for(auto i = 0; i < hierarchy.size(); i++){
    // //     if(hierarchy[i][3] == -1){
    // //         ext_contours.push_back(found_contours[i]);
    // //     }
    // // }





    // std::vector<std::vector<cv::Point2i>> approx;
    // // for(auto &e: ext_contours){
    // std::vector<cv::Point2i> a;
    // cv::approxPolyDP(found_contours[0], a, 1, true);
    // approx.push_back(a);
    // }
    
    //OCC CONTOURS

    cv::Mat occ_frontier = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);



    cv::Mat mask = this->rangeMask(x, y, scanned);

    cv::bitwise_or(this->occupancy_grid, mask, occ_frontier);

    std::vector<std::vector<cv::Point2i>> occ_contours;
    std::vector<cv::Vec4i> occ_hierarchy;
    cv::findContours(occ_frontier,occ_contours, occ_hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);


    // cv::drawContours(occ_frontier, occ_contours, -1, cv::Scalar(255));
    
    // cv::imshow("OCC Frontier map", occ_frontier);
    // cv::waitKey(0);

    //filter out non-external contours



    // for(auto i = 0; i < occ_hierarchy.size(); i++){
    //     if(occ_hierarchy[i][3] != -1){
    //         ext_occ_contours.push_back(ext_occ_contours[i]);
    //     }
    // }

    




    std::vector<std::vector<cv::Point2i>> occ_approx;
    for(auto &e: occ_contours){
        std::vector<cv::Point2i> a;
        cv::approxPolyDP(e, a, 1, true);
        occ_approx.push_back(a);
    }

    cv::drawContours(occ_frontier, occ_approx, -1, cv::Scalar(255));
    
    cv::imshow("OCC approx map", occ_frontier);
    cv::waitKey(0);



    
    return occ_approx;

}

std::pair<int,int> Agent::determineAction(){


    //Rastering Assumption
     
    //find furthest boundary

    std::pair<int,int> new_pos = this->coords;

    if(this->raster_step_direction == -1){ //need new direction
        std::vector<std::pair<int,int>> dir_dists;

        dir_dists.push_back(std::make_pair(0,0));
        for(int x = this->coords.first+1; x < this->field_x_width; x++){
            if(this->occupancy_grid.at<uint8_t>(this->coords.second, x) >= visited) break;
            dir_dists.back().second++;
        }
        
        dir_dists.push_back(std::make_pair(1,0));
        for(int y = this->coords.second+1; y < this->field_y_length; y++){
            if(this->occupancy_grid.at<uint8_t>(y, this->coords.first) >= visited) break;
            dir_dists.back().second++;
        }

        dir_dists.push_back(std::make_pair(2,0));
        for(int x = this->coords.first-1; x >=0 ; x--){
            if(this->occupancy_grid.at<uint8_t>(this->coords.second, x) >= visited) break;
            dir_dists.back().second++;
        }

        dir_dists.push_back(std::make_pair(3,0));
        for(int y = this->coords.second-1; y >=0 ; y--){
            if(this->occupancy_grid.at<uint8_t>(y, this->coords.first) >= visited) break;
            dir_dists.back().second++;
        }
            
        std::pair<int,int> result = *std::max_element(dir_dists.cbegin(), dir_dists.cend(), [](const std::pair<int,int> &lhs, const std::pair<int,int> &rhs) {
        return lhs.second < rhs.second;    
        });

        this->raster_step_direction = result.first;
        if(result.second == 0){
            exit(1);
        } 
    }

    std::cout << this->raster_step_direction << std::endl;

    switch (this->raster_step_direction)
    {
    case 0:
        new_pos.first++;
        if(new_pos.first +1 >= this->field_x_width || this->occupancy_grid.at<uint8_t>(new_pos.second, new_pos.first+1) >= visited) this->raster_step_direction = -1;
        break;
    case 1:
        new_pos.second++;
        if(new_pos.second +1 >= this->field_y_length || this->occupancy_grid.at<uint8_t>(new_pos.second+1, new_pos.first) >= visited) this->raster_step_direction = -1;
        break;
    case 2:
        new_pos.first--;
        if(new_pos.first -1 < 0 || this->occupancy_grid.at<uint8_t>(new_pos.second, new_pos.first-1) >= visited) this->raster_step_direction = -1;
        break;
    case 3:
        new_pos.second--;
        if(new_pos.second -1 <= 0 || this->occupancy_grid.at<uint8_t>(new_pos.second-1, new_pos.first) >= visited) this->raster_step_direction = -1;
        break;
    
    default:
        break;
    }

    return new_pos;






}

void Agent::moveToPosition(std::pair<int,int> pos){
    this->occupancy_grid.at<uint8_t>(this->coords.second, this->coords.first) = visited;
    pos.first = this->clipRange(0, this->field_x_width, pos.first);
    pos.second = this->clipRange(0, this->field_y_length, pos.second);
    this->coords = pos;
    this->occupancy_grid.at<uint8_t>(this->coords.second, this->coords.first) = blocked;
    this->coord_history.push_back(this->coords);
}



void Agent::measureSignalStrength(Field f) {
    std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords);
    // for(auto &m: measurements){
    //     std::cout << m.first << ": " << m.second << std::endl;
    // }
    this->measurements.push_back(measurements);
    
}





std::pair<int,int> Agent::getCoords(){
    return this->coords;
}

std::string Agent::logAgent() {
    std::ofstream agent_log;
    std::string file_path = "logs/" + this->name + "_agent_log.csv";
    agent_log.open(file_path);
    agent_log << "drone_x, drone_y";
    //this->measurements[0] is the number of sources
    for(int i = 0; i < this->measurements[0].size() ; i++){
        agent_log << ",id" << i << ",dist" << i;
    }
    agent_log << std::endl;
    
    //For each timestep

    auto it_coords = this->coord_history.begin();
    auto it_measurements = this->measurements.begin();
    while(it_coords != this->coord_history.end()  && it_measurements != this->measurements.end()){
        
        agent_log << (*it_coords).first << "," << (*it_coords).second;  
        for(auto &measurement: (*it_measurements)){
            agent_log << "," << measurement.first << "," << measurement.second;
        }
        agent_log << std::endl;
        it_coords++;
        it_measurements++;
        
    }
    
    return file_path;

}


void Agent::showOccGrid(){
    std::cout << "Occupancy Grid" << std::endl;
    std::cout << this->occupancy_grid << std::endl;

}

cv::Mat Agent::getOccGrid(){
    return this->occupancy_grid;
}

cv::Mat Agent::getFrontierMap(){
    cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);



    cv::drawContours(frontier_map, this->frontiers  , -1, cv::Scalar(255));
    
    return frontier_map;
}