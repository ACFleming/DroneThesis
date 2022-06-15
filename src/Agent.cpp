#include "Agent.hpp"


    /* matricies work in row, col referencing.
    rows = -y, cols = x
    y
    ^ 
    |
    |
    |
    |
    * - - - - - > x

    * - - - - - > cols
    |
    |
    |
    |
    |
    v
    rows

    for indexing use occupancy_grid[y][x]

    */





Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;
    this->field_x_width = field_width;
    this->field_y_length = field_length;
    this->scan_radius = scan_radius;

    this->started_iter_grid = false;
    
    
    int rows = this->field_y_length;
    int cols = this->field_x_width;
    this->occupancy_grid  = std::vector<std::vector<int>>(this->field_y_length, std::vector<int>(this->field_x_width, unknown));

    for(int y = 0; y < this->field_y_length; y++ ){
        for(int x = 0; x < this->field_x_width; x++){
            if(x == 0 || x == this->field_x_width-1 || y == 0 || y == this->field_y_length-1){
                this->occupancy_grid[y][x] = occupied;
            }
        }
    }


    //pair is x,y so index [y][x]
    this->occupancy_grid[this->coords.second][this->coords.first] = visited;


    



}

Agent::~Agent() {
    

}


// for this function the index are returned left to right, middle to top then middle to bottom. For most situation, the order is not important
int *Agent::nextGridSquare(){
    static int x;
    static int y;
    static int dy;
    static bool hit_top;
    static bool hit_bottom; 

    if(!this->started_iter_grid){
        this->started_iter_grid = true;
        //left edge of vision circle, clipped by the boundaries of the field;
        x = this->clipRange(0, this->field_x_width, this->coords.first - this->scan_radius); 
        y = this->coords.second;
        dy  = 0;
        hit_top = false;
        hit_bottom = false;
        return &(this->occupancy_grid[x][y]);
    }
    //goes all the way to the top;
    if(!hit_top &&  pow(x,2) + pow(y+dy,2) <= pow(this->scan_radius, 2) && (y+dy) < this->field_y_length){
            return &(this->occupancy_grid[x][y]);
            dy++;
    }else{
            hit_top = true;
            dy = 0;
    }
    if(!hit_bottom &&  pow(x,2) + pow(y-dy,2) <= pow(this->scan_radius, 2) && (y-dy) < this->field_y_length){
            return &(this->occupancy_grid[x][y]);
    }else{
            hit_top = true;
    }
    return nullptr;

}


//clips value in range such that lower <= value < upper
int Agent::clipRange(int lower, int upper, int value){
    if(value < lower){
        value = lower;
    }else if(value >= upper){
        value = upper;
    }
    return value;
}

// int Agent::markInRadius(occ_grid_vals value, bool justCount = false){
//     //for all x values from the current coord +- radius but also clipped by the boundary of the field
//     int counter = 0;
//     for(int x = this->clipRange(0, this->field_x_width, this->coords.first - this->scan_radius) ; x <= this->clipRange(0, this->field_x_width, this->coords.first+this->scan_radius); x++){
//         int dy = 0;
//         int y = this->coords.second;
//         bool hit_top = false;
//         bool hit_bottom = false;
//         while( pow(x,2) + pow(y+dy,2) <= pow(this->scan_radius, 2) ){
//             if(!hit_bottom && (y+dy) < this->field_y_length){
//                 counter++;
//                 if(justCount == false) this->occupancy_grid[x][y+dy] = value;
//                 return &(this->occupancy_grid[x][y+dy]);
                
//             }else{
//                 hit_top = true;
//             }
//             if(!hit_bottom && (y-dy) >= 0){
//                 counter++;
//                 if(justCount == false) this->occupancy_grid[x][y-dy] = value;
//             }else{
//                 hit_bottom = true;
//             }
//             dy++;
            
//         }
    
//     }
//     return counter;
// }


std::pair<int,int> Agent::determineAction(){
    //TODO
    //This is where the logic would be
    std::pair<int,int> next_position;



    








    
    return next_position;




}

void Agent::moveToPosition(std::pair<int,int> pos){
    pos.first = this->clipRange(0, this->field_x_width, pos.first);
    pos.second = this->clipRange(0, this->field_y_length, pos.second);
    this->coords = pos;
    this->coord_history.push_back(this->coords);
}



void Agent::measureSignalStrength(Field f) {
    this->measurements.push_back(f.getMeasurements(this->coords));
    
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
    for(auto &row : this->occupancy_grid){
        for(auto &col : row){
            std::cout << col;
        }
        std::cout << std::endl;
    }

}

std::vector<std::vector<int>> Agent::getOccGrid(){
    return this->occupancy_grid;
}