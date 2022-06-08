#include "Field.hpp"
#include "Agent.hpp"


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    Field f = Field(1000,1000,3, 10, 500);  
    Agent a1 = Agent("Drone1", 300, 600);
    
    for(int i = 0; i < 5; i++){
        a1.measureSignalStrength(f);
        a1.moveToPosition(a1.determineAction());
    }
    
    



    std::string a1_file_path = a1.logAgent();
    std::string field_file_path = f.logField();

    std::ofstream file_path_log;
    file_path_log.open("logs/file_path_log.csv");
    file_path_log << "File Paths" << std::endl;
    file_path_log << field_file_path << std::endl;
    file_path_log << a1_file_path << std::endl;
    file_path_log.close();



}