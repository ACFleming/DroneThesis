#ifndef FIELD_HPP
#define FIELD_HPP


#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <ctime>
#include <math.h>

class Field
{
private:
    int x_width;
    int y_height;
    int n_sources;
    double std_dev;
    double range_cap;
    std::vector<std::pair<int,int>> signal_sources;
    
    
public:
    Field(int x_width, int y_height, int n_sources);
    Field(int x_width, int y_height, int n_sources, double std_dev, double range_cap);
    ~Field();
    std::vector<std::pair<int,int>> getSources();
    std::vector<double> getMeasurements(std::pair<int,int> pos);
    void logField();

    
};







#endif