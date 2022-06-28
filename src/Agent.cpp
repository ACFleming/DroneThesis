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
    this->raster_step_count = 0;
    
    


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
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x + sin(PI/4)*this->scan_radius))  ,this->clipRange(0, this->field_y_length, int(y - sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
    //E
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x + this->scan_radius))             ,this->clipRange(0, this->field_y_length, int(y)));
    clockwise_points.push_back(p);
    //SE
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x + sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y + sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
    //S
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x))                                 ,this->clipRange(0, this->field_y_length, int(y + this->scan_radius)));
    clockwise_points.push_back(p);
    //SW
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x - sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y + sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
    //W
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x - this->scan_radius))             ,this->clipRange(0, this->field_y_length, int(y)));
    clockwise_points.push_back(p);
    //NW
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x - sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y - sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
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



    this->frontiers = this->getNewFrontiers(this->coords.first, this->coords.second);
    std::cout << "Frontier count: " << this->frontiers.size() << std::endl; 




}

std::vector<cv::Point2i> Agent::getNewFrontiers(int x, int y){
    std::vector<cv::Point2i> oct = this->getOctagonPoints(x, y);



    cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);
    
    std::vector<std::vector<cv::Point2i>> new_frontiers;
    new_frontiers.push_back(oct);
    if(!this->frontiers.empty()) new_frontiers.push_back(this->frontiers);

    cv::drawContours(frontier_map, new_frontiers, -1, cv::Scalar(255));
    
    // cv::imshow("Frontier map", frontier_map);
    // cv::waitKey(0);


    
    std::vector<std::vector<cv::Point2i>> found_contours;

    cv::findContours(frontier_map,found_contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

    if(found_contours.size() > 1){
        exit(1);
    }

    std::vector<cv::Point2i> approx;
    cv::approxPolyDP(found_contours[0], approx, 1, true);


    
    return approx;

}

std::pair<int,int> Agent::determineAction(){


    //Rastering Assumption
    //Range of scan >= speed




    std::pair<int,int> next_position = this->coords;

    next_position.first +=50;

    
    std::vector<std::pair<cv::Point2i,double>> cost_vec;

    int curr_scanned = cv::countNonZero(this->occupancy_grid);
    int curr_frontier_count = this->frontiers.size();

    // std::vector<cv::Point2i> frontiers = this->getNewFrontiers();
    cv::Mat new_cells;
    std::vector<cv::Point2i> new_frontiers;
    for(auto &f: this->frontiers ){
        cv::bitwise_or(this->occupancy_grid, this->rangeMask(f.x, f.y, 200), new_cells);
        int new_scanned_count = cv::countNonZero(new_cells) - curr_scanned;

        
        
        cv::imshow("New cells", new_cells);
        cv::waitKey(0);

        new_frontiers = this->getNewFrontiers(f.x, f.y);
        int new_frontier_count = new_frontiers.size() - curr_frontier_count;




        cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);
        std::vector<std::vector<cv::Point2i>> ctr;
        ctr.push_back(new_frontiers);
        cv::drawContours(frontier_map, ctr, -1, cv::Scalar(255));
        cv::imshow("New frontiers", frontier_map);
        cv::waitKey(0);

        int dist = int(hypot(this->coords.first - f.x, this->coords.second-f.y));

        double score = -100*new_frontier_count + 0.1*new_scanned_count - 1*dist;
        std::cout << "NSC: " << new_scanned_count << " NFC: " << new_frontier_count << " D: " << dist << " Score: " << score << std::endl;

        cost_vec.push_back(std::make_pair(f, score));
        
    }


    //Finf min arg
    std::pair<cv::Point2i,double>result = *std::max_element(cost_vec.cbegin(), cost_vec.cend(), [](const std::pair<cv::Point2i,double> &lhs, const std::pair<cv::Point2i,double> &rhs) {
        return lhs.second < rhs.second;    
    });


    std::cout <<  "BEST! " << "Point:" << result.first.x <<"," << result.first.y << " Score: " << result.second << std::endl;



    return std::make_pair(result.first.x, result.first.y);


}

void Agent::moveToPosition(std::pair<int,int> pos){
    this->occupancy_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
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

    std::vector<std::vector<cv::Point2i>> f;
    f.push_back(this->frontiers);

    cv::drawContours(frontier_map, f, -1, cv::Scalar(255));
    
    return frontier_map;
}