#include "Field.hpp"


Field::Field(int x_width, int y_length, int n_source) : Field(x_width, y_length, n_source, 5, 100) {
}

Field::Field(int x_width, int y_length, int n_sources, double std_dev, double range_cap) {
    this->x_width = x_width;
    this->y_length = y_length;
    this->std_dev = std_dev;
    this->range_cap = range_cap;



    if(n_sources == 0){
        this->signal_sources.push_back(SignalSource(int(x_width/2),int(y_length/2),"DEAD CENTRE"));
    }else{

        //place all the random sources
        for(int i = 0; i < n_sources; i++){
            this->signal_sources.push_back(SignalSource(this->x_width, this->y_length));
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
    //seed random generator with time
    std::default_random_engine generator(time(0));
    for(auto &source : this->signal_sources){
        //hypot(x,y) returns sqrt(x^2 + y^2). Finding true euclidian distance
        std::pair<int,int> s_coords = source.getCoords();
        double true_distance = hypot(s_coords.first-pos.first, s_coords.second-pos.second); 
        //generating sensor reading using gaussian noise assumption. mean is true measurement. std dev is set manually on creation.
        
        std::normal_distribution<double> distribution(true_distance,this->std_dev);
        double dist_with_noise = distribution(generator);
        // cant have negative distance
        if(dist_with_noise < 0){ dist_with_noise = 0;}
        // if too far away no signal detected
        if(dist_with_noise > this->range_cap){ dist_with_noise = 0;}
        distances.push_back(std::make_pair(source.getId(), dist_with_noise));
    }
    return distances;
}

void Field::logField() {
    std::ofstream field_log;
    field_log.open("logs/field_log_file.csv");
    field_log << "First row is x_width and y_length of the field. The rest are the x and y coords of the sources" << std::endl;
    field_log << this->x_width << "," << this->y_length << std::endl;
    for(auto &source: this->signal_sources){
        std::pair<int,int> s_coords = source.getCoords();
        field_log << s_coords.first << "," << s_coords.second << std::endl;
    }
}
