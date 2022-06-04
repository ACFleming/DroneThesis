#include "Agent.hpp"


Agent::Agent(std::string name, int x_coord, int y_coord) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->name = name;

}

Agent::~Agent() {
    
}

void Agent::measureSignalStrength(Field f) {
    this->measurements.push_back(f.getMeasurements(this->coords));
}

void Agent::logAgent() {
    std::ofstream agent_log;
    agent_log.open(this->name + "_agent_file.csv");
    agent_log << "Cols: x,y, id1, dist1, id2, dist2, etc. " << std::endl;
    agent_log << this->coords.first << "," << this->coords.second;
    for(auto &timestep: this->measurements){
        for(auto &measurement: timestep){
            agent_log << "," << measurement.first << "," << measurement.second;
        }
        agent_log << std::endl;
    }
}
