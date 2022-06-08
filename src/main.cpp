#include "Field.hpp"
#include "Agent.hpp"


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    Field f = Field(2000,1000,1, 0, 2000);  
    Agent a1 = Agent("Drone1", 300, 600);
    Agent a2 = Agent("Drone2", 750, 750);
    
    for(int i = 0; i < 20; i++){
        a1.measureSignalStrength(f);
        a1.moveToPosition(a1.determineAction());
        a2.measureSignalStrength(f);
        a2.moveToPosition(a2.determineAction());
    }
    
    



    std::string a1_file_path = a1.logAgent();
    std::string a2_file_path = a2.logAgent();
    std::string field_file_path = f.logField();

    std::ofstream file_path_log;
    file_path_log.open("logs/file_path_log.csv");
    file_path_log << "File Paths" << std::endl;
    file_path_log << field_file_path << std::endl;
    file_path_log << a1_file_path << std::endl;
    file_path_log << a2_file_path << std::endl;
    file_path_log.close();



}