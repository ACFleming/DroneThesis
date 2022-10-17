#include "Field.hpp"


Field::Field(int x_rows, int y_cols, int n_source) : Field(x_rows, y_cols, n_source, 5, 100) {
}

Field::Field(int x_rows, int y_cols, int n_sources, double std_dev, double range_cap) {
    this->x_rows = x_rows;
    this->y_cols = y_cols;
    this->std_dev = std_dev;
    this->range_cap = range_cap;
    

    

    if(n_sources < 0){
        this->signal_sources.push_back(SignalSource(int(0.5*x_rows),int(0.1*y_cols),"DEAD CENTRE"));
    }else{

        //place all the random sources
        std::vector<SignalSource> mt;
        this->signal_sources = mt;
        for(int i = 0; i < n_sources; i++){
            this->signal_sources.push_back(SignalSource(this->x_rows, this->y_cols));
        }
    }
    
}

Field::~Field() {
    this->signal_sources.clear();
}

std::vector<SignalSource> Field::getSources() {
    return this->signal_sources;
}

std::vector<std::pair<std::string,double>> Field::getMeasurements(std::pair<int, int> pos) {
    std::vector<std::pair<std::string,double>> distances;
    //seed random generator with random
    std::default_random_engine generator(rand());
    for(auto &source : this->signal_sources){
        //hypot(x,y) returns sqrt(x^2 + y^2). Finding true euclidian distance
        std::pair<int,int> s_coords = source.getCoords();
        double true_distance = hypot(s_coords.first-pos.first, s_coords.second-pos.second); 
        if(true_distance > this->range_cap){ 
            continue;
        }
        std::cout << "True: " << true_distance; 


        //generating sensor reading using gaussian noise assumption. mean is true measurement. std dev is set manually on creation.
        
        std::normal_distribution<double> distribution(true_distance,this->std_dev);
        

        // double dist_with_noise = distribution(generator);
        double dist_with_noise = true_distance;

        std::cout << "Noisy: " <<  dist_with_noise << std::endl;

        // cant have negative distance
        if(dist_with_noise < 0) dist_with_noise = 0;
        // if too far away no signal detected
        if(dist_with_noise > this->range_cap){ 
            continue;
        }
        distances.push_back(std::make_pair(source.getId(), dist_with_noise));
    }
    return distances;
}

void Field::logField(std::ostream* output_file) {
    *output_file << "Name, x, y" << std::endl;
    *output_file << "Field Dimensions"  << "," << this->x_rows << "," << this->y_cols <<  std::endl;
    for(auto &source: this->signal_sources){
        *output_file << source.getId() << "," << source.getCoords().first << "," << source.getCoords().second << std::endl;
    }
}
