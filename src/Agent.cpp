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

cv::Mat Agent::rangeMask(cv::Mat image, int x, int y){
    cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
    cv::Point centre(x,y);
    cv::circle(mask, centre, this->scan_radius, cv::Scalar(255,255,255), -1);
    return mask;
}


std::vector<cv::Point2i> Agent::gridSquaresInRange(int x, int y){

    std::vector<cv::Point2i> locations;

    cv::Mat mask = this->rangeMask(this->occupancy_grid, x, y);


    cv::findNonZero(mask, locations);

    return locations;

}

std::vector<cv::Point2i> Agent::gridSquaresInRange(std::pair<int,int> coords){
    return this->gridSquaresInRange(coords.first, coords.second);
}

std::vector<cv::Point2i> Agent::globalFrontiers(){


    

    cv::Mat frontier_edges;
    // this->occupancy_grid.copyTo(frontier_edges);

    cv::threshold(this->occupancy_grid,frontier_edges, scanned, blocked, cv::THRESH_TOZERO_INV );
    //filling the hole at the centre of being scanned
    frontier_edges.at<uint8_t>(this->coords.second,this->coords.first) = scanned;




    cv::Canny(frontier_edges, frontier_edges, blocked, scanned);



    cv::imshow("Frontiers", frontier_edges);
    cv::waitKey(10);
    
    std::vector<cv::Point2i> locations;


    cv::findNonZero(frontier_edges, locations);

    // for(auto &p : locations){
    //     std::cout << p << std::endl;
    // }
    return locations;

}

std::pair<int,int> Agent::determineAction(){


    //Rastering Assumption
    //Range of scan >= speed




    std::pair<int,int> next_position = this->coords;


    
    std::vector<std::pair<cv::Point2i,double>> cost_vec;


    std::vector<cv::Point2i> frontiers = this->globalFrontiers();


    //For each frontier point, find how many new grid squares would be uncovered
    for(auto &f: frontiers){
        std::vector<cv::Point2i> grid_in_range = this->gridSquaresInRange(f.x, f.y);
        int unknown_counter = 0;
        for(auto &p: grid_in_range){
            // if(this->coords.first == 508) {
            //     std::cout << p.x << " " <<  p.y << std::endl;
            // }
        if(this->occupancy_grid.at<uint8_t>(p) == unknown) unknown_counter++;
        }

        

        int dist = int(hypot(this->coords.first - f.x, this->coords.second-f.y)); 
        //Cost function
        // std::cout << "Point:" << f.x <<"," << f.y << " Dist: " << dist << " Unknown count: " << unknown_counter << std::endl;
        double cost = dist*1 -0.01*unknown_counter;
        cost_vec.push_back(std::make_pair(f, cost));
        
    }


    //Finf min arg
    std::pair<cv::Point2i,double>result = *std::min_element(cost_vec.cbegin(), cost_vec.cend(), [](const std::pair<cv::Point2i,double> &lhs, const std::pair<cv::Point2i,double> &rhs) {
        return lhs.second < rhs.second;    
    });
        

    std::cout <<  "BEST! " << "Point:" << result.first.x <<"," << result.first.y << " Cost: " << result.second << std::endl;

    //move in direction of this point

    double mul_factor = this->speed/(hypot(this->coords.first - result.first.x, this->coords.second- result.first.y)); 
    int x_coord = this->coords.first + int( (result.first.x-this->coords.first)*mul_factor);
    int y_coord = this->coords.second + int(  (result.first.y-this->coords.second)*mul_factor);
    std::cout << "Mul factor: " << mul_factor <<  " Target: " << x_coord << ", " << y_coord << std::endl;
    std::cout << "Dist of target point: " << int(hypot(this->coords.first - x_coord, this->coords.second-y_coord)) << std::endl;



    return std::make_pair(x_coord,y_coord);



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
    std::vector<cv::Point> locations = this->gridSquaresInRange(this->coords);
    for(auto &p: locations){
        if(this->occupancy_grid.at<uint8_t>(p) < scanned){
            this->occupancy_grid.at<uint8_t>(p) = scanned;
        }
    }
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