#include "Field.hpp"
#include "Agent.hpp"


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    int field_x_width = 200;
    int field_y_length = 100;
    int num_sources = 3;
    int std_dev_noise = 10;
    int max_range = 1000;
    Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
    Agent a1 = Agent("Drone1", 0.1*field_x_width, 0.1*field_y_length, field_x_width, field_y_length, max_range);
    // Agent a2 = Agent("Drone2", 0.75*field_x_width, 0.75*field_y_length, field_x_width, field_y_length);
    
    std::cout << "Showing grid" << std::endl;
    a1.showOccGrid();
    
    for(int i = 0; i < 30; i++){
        a1.measureSignalStrength(f);
        a1.moveToPosition(a1.determineAction());
        
        // a2.measureSignalStrength(f);
        // a2.moveToPosition(a2.determineAction());
    }
    
    



    std::string a1_file_path = a1.logAgent();
    // std::string a2_file_path = a2.logAgent();
    std::string field_file_path = f.logField();

    std::ofstream file_path_log;
    file_path_log.open("logs/file_path_log.csv");
    file_path_log << "File Paths" << std::endl;
    file_path_log << field_file_path << std::endl;
    file_path_log << a1_file_path << std::endl;
    // file_path_log << a2_file_path << std::endl;
    file_path_log.close();



}