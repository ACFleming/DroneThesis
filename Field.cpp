#include "Field.hpp"


Field::Field(int x_width, int y_height, int n_source) : Field(x_width, y_height, n_source, 10, 100) {
}

Field::Field(int x_width, int y_height, int n_sources, double std_dev, double range_cap) {
    this->x_width = x_width;
    this->y_height = y_height;
    this->std_dev = std_dev;
    this->range_cap = range_cap;

    //seed the random number generator
    srand(time(0));

    //place all the random sources
    for(int i = 0; i < n_sources; i++){
        
        int source_x = std::rand() % this->x_width;
        int source_y = std::rand() % this->y_height;
        std::cout << "X: " << source_x << " Y: " << source_y << std::endl;
        std::pair<int,int> new_pair = {source_x, source_y};
        this->signal_sources.push_back(new_pair);
        // std::cout << this->signal_sources.size() << std::endl;
    }
}

Field::~Field() {
    this->signal_sources.clear();
}

std::vector<std::pair<int,int>> Field::getSources() {
    return this->signal_sources;
}

std::vector<double> Field::getMeasurements(std::pair<int, int> pos) {
    std::vector<double> distances;
    std::default_random_engine generator;
    // std::cout << this->signal_sources.size() << std::endl;
    for(auto &s: this->signal_sources){
        // std::cout << "GETTNG MEASUREMENTS" << std::endl;
        double true_distance = hypot(s.first-pos.first, s.second-pos.second); 
        std::normal_distribution<double> distribution(true_distance,this->std_dev);
        distances.push_back(distribution(generator));
    }
    return distances;
}

void Field::logField() {
    std::ofstream field_log;
    field_log.open("field_log_file.csv");
    field_log << "First row is x_width and y_height of the field. The rest are the x and y coords of the sources" << std::endl;
    field_log << this->x_width << "," << this->y_height << std::endl;
    for(auto &s: this->signal_sources){
        field_log << s.first << "," << s.second << std::endl;
    }
}
