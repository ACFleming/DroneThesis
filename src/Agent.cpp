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

Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;
    this->field_x_width = field_width;
    this->field_y_length = field_length;
    this->scan_radius = scan_radius;

    this->started_iter_grid = false;
    
    


    this->occupancy_grid = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC1);

        
    // std::cout << this->occupancy_grid << std::endl;

    cv::Rect boundary(0,0, this->field_x_width-1, this->field_y_length-1);

    cv::rectangle(this->occupancy_grid, boundary, cv::Scalar(blocked, blocked, blocked));


    std::cout << this->occupancy_grid.depth() << std::endl;

    this->occupancy_grid.at<uint8_t>(this->coords.second,this->coords.first) = blocked;




    



}

Agent::~Agent() {
    

}




// // for this function the index are returned left to right, middle to top then middle to bottom. For most situation, the order is not important
// int *Agent::nextGridSquare(){
//     static int x;
//     static int y;
//     static int dy;
//     static bool hit_top;
//     static bool hit_bottom; 

//     if(!this->started_iter_grid){
//         this->started_iter_grid = true;
//         //left edge of vision circle, clipped by the boundaries of the field;
//         x = this->clipRange(0, this->field_x_width, this->coords.first - this->scan_radius); 
//         y = this->coords.second;
//         dy  = 0;
//         hit_top = false;
//         hit_bottom = false;
//         return &(this->occupancy_grid[x][y]);
//     }
//     //goes all the way to the top;
//     if(!hit_top &&  pow(x,2) + pow(y+dy,2) <= pow(this->scan_radius, 2) && (y+dy) < this->field_y_length){
//             return &(this->occupancy_grid[x][y]);
//             dy++;
//     }else{
//             hit_top = true;
//             dy = 0;
//     }
//     if(!hit_bottom &&  pow(x,2) + pow(y-dy,2) <= pow(this->scan_radius, 2) && (y-dy) < this->field_y_length){
//             return &(this->occupancy_grid[x][y]);
//     }else{
//             hit_top = true;
//     }
//     return nullptr;

// }


//clips value in range such that lower <= value < upper
int Agent::clipRange(int lower, int upper, int value){
    if(value < lower){
        value = lower;
    }else if(value >= upper){
        value = upper;
    }
    return value;
}




std::vector<cv::Point2i> Agent::getAllGridSquares(int x, int y){

    std::vector<cv::Point2i> locations;

    cv::Mat mask = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC1);

    cv::Point centre(x,y);
    cv::circle(mask, centre, this->scan_radius, cv::Scalar(255,255,255), -1);


    cv::findNonZero(mask, locations);

    return locations;

}

std::vector<cv::Point2i> Agent::getAllGridSquares(std::pair<int,int> coords){
    return this->getAllGridSquares(coords.first, coords.second);
}



std::pair<int,int> Agent::determineAction(){
    //TODO
    //This is where the logic would be
    std::pair<int,int> next_position = this->coords;

    next_position.first += 10;   
    return next_position;




}

void Agent::moveToPosition(std::pair<int,int> pos){
    pos.first = this->clipRange(0, this->field_x_width, pos.first);
    pos.second = this->clipRange(0, this->field_y_length, pos.second);
    this->coords = pos;
    this->coord_history.push_back(this->coords);
}



void Agent::measureSignalStrength(Field f) {
    std::vector<cv::Point> locations = this->getAllGridSquares(this->coords);
    for(auto &p: locations){
        if(this->occupancy_grid.at<uint8_t>(p) < scanned){
            this->occupancy_grid.at<uint8_t>(p) = scanned;
        }
    }

    this->measurements.push_back(f.getMeasurements(this->coords));
    
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