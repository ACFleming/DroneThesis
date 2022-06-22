#pragma once


#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <math.h>
#include <chrono>
#include "SignalSource.hpp"


class Field
{
private:
    int x_width;
    int y_length;
    double std_dev;
    double range_cap;
    std::vector<SignalSource> signal_sources;
    
    
    
public:
    Field(int x_width, int y_length, int n_sources);
    Field(int x_width, int y_length, int n_sources, double std_dev, double range_cap);
    ~Field();
    std::vector<SignalSource> getSources();
    std::vector<std::pair<std::string,double>> getMeasurements(std::pair<int,int> pos);
    std::string logField();

    
};



