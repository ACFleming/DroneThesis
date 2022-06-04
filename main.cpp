#include "Field.hpp"


int main (int argc, char* argv[]){
    Field f = Field(250,300,25);
    std::cout << f.getSources().size() << std::endl;
    

    std::pair<int,int> drone_pos = {10,10};
    std::vector<double> bt_measurements =  f.getMeasurements(drone_pos);

    for(auto &bt : bt_measurements){
        
        std::cout << bt << std::endl;
    }
    
    f.logField();
}