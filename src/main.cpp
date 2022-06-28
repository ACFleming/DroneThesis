#include "Field.hpp"
#include "Agent.hpp"
#include <opencv2/core.hpp>


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    int field_x_width = 900;
    int field_y_length = 600;
    int num_sources = 3;
    int std_dev_noise = 10;
    int max_range = 50;
    int speed = 50;
    int bearing = 90;


    // std::cout << cv::getBuildInformation() << std::endl;

    Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
    Agent a1 = Agent("Drone1", 0, 0, field_x_width, field_y_length, max_range,speed, bearing);


    //RUN FUNCTION

    while(true){
        a1.measureSignalStrength(f);
        a1.updateScannedGrid();

        cv::imshow("OCC GRID", a1.getOccGrid());
        cv::waitKey(0);

        
    
        cv::imshow("Frontier map", a1.getFrontierMap());
        cv::waitKey(0);


        std::pair<int,int> dst = a1.determineAction();
        a1.moveToPosition(dst);
    }



}