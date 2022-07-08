#include "Field.hpp"
#include "Agent.hpp"
#include "Ring.hpp"
#include <opencv2/core.hpp>


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    int field_x_width = 400;
    int field_y_length = 400;
    int num_sources = 3;
    int std_dev_noise = 5;
    int max_range = 30;
    int speed = 30;
    int bearing = 90;


    // std::cout << cv::getBuildInformation() << std::endl;

    Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
    Agent a1 = Agent("Drone1", 50, 0, field_x_width, field_y_length, max_range,speed, bearing);


    // Ring r1 = Ring(field_x_width, field_y_length, 10, 90, sqrt(3400), 10);
    // r1.drawRing();
    // cv::Mat m1 = r1.getCanvas();

    // cv::imshow("m1", m1);
    // cv::waitKey(0);


    // Ring r2 = Ring(field_x_width, field_y_length, 30, 90, sqrt(2600), 10);
    // r2.drawRing();
    // cv::Mat m2 = r2.getCanvas();

    // cv::imshow("m2", m2);
    // cv::waitKey(0);


    // Ring r3 = Ring(field_x_width, field_y_length, 40, 110, sqrt(4900), 10);
    // r3.drawRing();
    // cv::Mat m3 = r3.getCanvas();

    // cv::imshow("m3", m3);
    // cv::waitKey(0);


    // std::vector<Ring> rings = {r1, r2, r3};
    // cv::Mat m4 = Ring::intersectRings(rings);

    // cv::imshow("m4", m4);
    // cv::waitKey(0);

    // Ring::dummy();    
    // std::vector<std::pair<bool,cv::Point>> centres  = Ring::centroidOfIntersections(m4);
    // for(auto&c: centres){
    //     cv::circle(m4, c.second, 3, cv::Scalar(127),-1);
    // }

    
    // cv::imshow("m4", m4);
    // cv::waitKey(0);

    // int i = 7;
    

    // RUN FUNCTION

    cv::Mat path = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);



    cv::imshow("Path", path);
    cv::waitKey(0);



    std::pair<int,int> curr = a1.getCoords();
    int counter = 0;

    while(true){
        cv::circle(path, cv::Point2i(curr.first,curr.second),2,cv::Scalar(0,255,255));
        a1.measureSignalStrength(f);
        a1.updateScannedGrid();

        std::pair<int,int> pos = a1.getCoords();
        cv::Mat occ_grid = a1.getOccGrid();
        std::cout << "Agent at: " << pos.first << "," << pos.second << std::endl;




        cv::line(path, cv::Point2i(curr.first, curr.second), cv::Point2i(pos.first,pos.second),cv::Scalar(255, 255-counter, counter));

        curr = pos;
        counter +=1;

        cv::Mat img = cv::Mat(occ_grid.size(), CV_8UC3);
        cv::cvtColor(occ_grid, occ_grid, cv::COLOR_GRAY2BGR);


        cv::bitwise_or(path, occ_grid, img);

        cv::imshow("Path", img);
        cv::waitKey(10);



        
    
        // cv::imshow("Frontier map", a1.getFrontierMap());
        // cv::waitKey(0);
        


        std::pair<int,int> dst = a1.determineAction();
        a1.moveToPosition(dst);



    }



}