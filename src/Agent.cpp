#include "Agent.hpp"


Agent::Agent(std::string name, int x_coord, int y_coord) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;

}

Agent::~Agent() {
    
}


void Agent::takeAction(){
    //TODO
    //This is where the logic would be

    this->coords = std::make_pair(this->coords.first+30, this->coords.second);
    this->coord_history.push_back(this->coords);


}

void Agent::measureSignalStrength(Field f) {
    this->measurements.push_back(f.getMeasurements(this->coords));
}

void Agent::logAgent() {
    std::ofstream agent_log;
    agent_log.open("logs/" + this->name + "_agent_file.csv");
    agent_log << "Cols: x,y, id1, dist1, id2, dist2, etc. " << std::endl;
    
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
    
    

}
