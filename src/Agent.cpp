#include "Agent.hpp"




Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;
    this->field_x_width = field_width;
    this->field_y_length = field_length;

}

Agent::~Agent() {
    
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


std::pair<int,int> Agent::determineAction(){
    //TODO
    //This is where the logic would be
    std::pair<int,int> next_position;

    if(this->name == "Drone1"){
        next_position.first = this->coords.first+30;
        next_position.second = this->coords.second;
    }
    if(this->name == "Drone2"){
        next_position.first = this->coords.first;
        next_position.second = this->coords.second-30;
    }



    
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
