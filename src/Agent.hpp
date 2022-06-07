#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "Field.hpp"

class Agent
{
private:
    std::pair<int,int> coords;
    std::vector<std::pair<int,int>> coord_history;
   
    std::string name;
    int max_sources;

    // each pair is an id-measurement pair. Multiple of these pairs are measured each time step, collected in the inside vector.
    // the outside vector is a collection of all timesteps' measurements. 
    std::vector<std::vector<std::pair<std::string,double>>> measurements;
public:
    Agent(std::string name, int x_coord, int y_coord);
    ~Agent();

    void takeAction();
    void measureSignalStrength(Field f);
    void logAgent();
};


