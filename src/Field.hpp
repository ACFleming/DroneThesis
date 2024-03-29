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
    int x_rows;
    int y_cols;
    double std_dev;
    double range_cap;
    std::vector<SignalSource> signal_sources;
    
    
    
public:
    Field(int x_rows, int y_cols, int n_sources);
    Field(int x_rows, int y_cols, int n_sources, double std_dev, double range_cap);
    ~Field();
    std::vector<SignalSource> getSources();
    std::vector<std::pair<std::string,double>> getMeasurements(std::pair<int,int> pos);
    void logField(std::ostream* output_file);

    
};



