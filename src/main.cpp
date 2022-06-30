#include "Field.hpp"
#include "Agent.hpp"
#include <opencv2/core.hpp>


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    int field_x_width = 100;
    int field_y_length = 100;
    int num_sources = 3;
    int std_dev_noise = 10;
    int max_range = 2;
    int speed = max_range/2;
    int bearing = 90;


    // std::cout << cv::getBuildInformation() << std::endl;

    Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
    Agent a1 = Agent("Drone1", 10, 0, field_x_width, field_y_length, max_range,speed, bearing);


    //RUN FUNCTION

    cv::Mat path = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);
    cv::Mat disp;


    std::pair<int,int> curr = a1.getCoords();
    int b = 200;
    int g = 30;
    int r = 200;


    while(true){
        a1.measureSignalStrength(f);
        a1.updateScannedGrid();

        cv::resize(path,disp,cv::Size(), 10, 10 );
        cv::imshow("Path", disp);
        cv::waitKey(0);

        
    
        // cv::imshow("Frontier map", a1.getFrontierMap());
        // cv::waitKey(0);


        


        std::pair<int,int> dst = a1.determineAction();
        a1.moveToPosition(dst);
        std::pair<int,int> pos = a1.getCoords();
        std::cout << "Agent at: " << pos.first << "," << pos.second << std::endl;



        cv::line(path, cv::Point2i(curr.first, curr.second), cv::Point2i(pos.first,pos.second),cv::Scalar(b,g,r));
        curr = pos;
        // if(g < 255){
        //     g += 1;
        // }else{
        //     g = 0;
        //     r += 5;
        // }

    }



}


