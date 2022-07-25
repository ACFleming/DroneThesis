#include "Field.hpp"
#include "Agent.hpp"
#include "Ring.hpp"
#include <opencv2/core.hpp>


int main (int argc, char* argv[]){
    //seed the random number generator
    srand(time(0));
    int field_x_width = 300;
    int field_y_length = 300;
    int num_sources = 2;
    int std_dev_noise = 3;
    int max_range = 30;
    int speed = 30;
    int bearing = 90;




    Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
    Agent a1 = Agent("Drone1", 0, 0, field_x_width, field_y_length, max_range,speed, bearing);


    // cv::Mat gray = cv::Mat::ones(field_y_length, field_x_width, CV_8UC1)*127;




    // // cv::circle(gray, cv::Point2i(200,200), 20, cv::Scalar(255), 0);

    // cv::imshow("Gray", gray);
    // cv::waitKey(0);



    // cv::Mat m2;
    
    // cv::Mat m1;
    // // cv::bitwise_and(gray, m1, m2);
    // for(int i = 0; i< 1; i++){
    //     Ring r1 = Ring(field_x_width, field_y_length, 50, 50, 50, -1);
    //     r1.drawRing();
    //     m1 = r1.getCanvas();
    //     cv::addWeighted(gray, 0, m1, 1, 0, gray);

    //     cv::imshow("gray", gray);
    //     cv::waitKey(0);

    // }


    // cv::imshow("m2", m2);
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

    cv::Mat map = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);


    // std::pair<int,int> c = f.getSources()[0].getCoords();
    for(auto &c: f.getSources()){
        cv::circle(map, cv::Point2i(c.getCoords().first, c.getCoords().second), 3, cv::Scalar(255,0,255) );
    }
    std::cout << "HERE2" << std::endl;



    cv::imshow("map", map);
    cv::waitKey(0);





    std::pair<int,int> curr = a1.getCoords();
    int counter = 0;

    while(true){
        cv::circle(map, cv::Point2i(curr.first,curr.second),2,cv::Scalar(0,255,255));
        std::pair<int,int> p = a1.updateCertainty(f);


        std::pair<int,int> pos = a1.getCoords();
        cv::Mat cert_grid = a1.getCertGrid();
        std::cout << "Agent at: " << pos.first << "," << pos.second << std::endl;




        // cv::imshow("Certainty grid", cert_grid);
        // cv::waitKey(0);



        cv::line(map, cv::Point2i(curr.first, curr.second), cv::Point2i(pos.first,pos.second),cv::Scalar(255, 255-counter, counter));

        curr = pos;
        counter +=1;

        cv::Mat img = cv::Mat(cert_grid.size(), CV_8UC3);
        cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);

        cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
        // cv::bitwise_or(map, cert_grid, img);

        cv::imshow("map", img);
        cv::waitKey(10);



        
    
        // cv::imshow("Frontier map", a1.getFrontierMap());
        // cv::waitKey(0);
        
        if(p.first == -1){
            std::pair<int,int> dst = a1.determineAction();
            a1.moveToPosition(dst);
        }else{
            a1.moveToPosition(p);
        }





    }



}