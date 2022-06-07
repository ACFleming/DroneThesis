#include "Field.hpp"
#include "Agent.hpp"


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    Field f = Field(1000,1000,15, 10, 500);  
    Agent a1 = Agent("Drone1", 300, 600);
    a1.measureSignalStrength(f);
    



    a1.logAgent();
    f.logField();
}