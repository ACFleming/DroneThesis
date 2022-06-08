#include "Agent.hpp"


Agent::Agent(std::string name, int x_coord, int y_coord) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;

}

Agent::~Agent() {
    
}


std::pair<int,int> Agent::determineAction(){
    //TODO
    //This is where the logic would be
    std::pair<int,int> next_position;

    next_position.first = this->coords.first+30;
    next_position.second = this->coords.second;
    
    return next_position;




}

void Agent::moveToPosition(std::pair<int,int> pos){
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
