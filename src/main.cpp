#include "Field.hpp"
#include "Agent.hpp"
#include "Ring.hpp"
#include "Grid.hpp"
#include <opencv2/core.hpp>
#include <map>
#include <string>


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

    std::map<std::string, Grid> certainty_grids = std::map<std::string, Grid>();


    // cv::Mat image = cv::Mat(300,300,CV_8UC1, cv::Scalar(200));

    // cv::Mat image2 = cv::Mat(300,300,CV_8UC1, cv::Scalar(0));

    // cv::circle(image2, cv::Point(150,150), 50, cv::Scalar(255), -1);

    // cv::bitwise_not(image2, image2);
    // cv::bitwise_and(image,image2, image);
    



    // cv::imshow("and", image);
    // cv::waitKey(0);






    Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
    Agent a1 = Agent("Drone1", 0 , 0, field_x_width, field_y_length, max_range,speed, &certainty_grids);
    Agent a2 = Agent("Drone2", 299 ,299, field_x_width, field_y_length, max_range,speed, &certainty_grids);

    

    // RUN FUNCTION

    cv::Mat map = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);
    cv::Mat img = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);



    //Display true location
    for(auto &c: f.getSources()){
        cv::circle(map, cv::Point2i(c.getCoords().first, c.getCoords().second), 3, cv::Scalar(255,0,255) );
    }






    std::pair<int,int> a1_curr = a1.getCoords();    
    cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));  

    std::pair<int,int> a2_curr = a2.getCoords();
    cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
        
    cv::imshow("map", map);
    cv::waitKey(10);

    int counter = 0;

    while(true){

        

        //plot a1 

        std::pair<int,int> a1_curr = a1.getCoords();
        std::cout << "Drone1 at: " << a1_curr.first << "," << a1_curr.second << std::endl;
        a1.updateCertainty(f);    
        std::pair<int,int> a1_dest = a1.determineAction();
        a1_dest = a1.moveToPosition(a1_dest);


        cv::line(map, a1.pair2Point(a1_dest), a1.pair2Point(a1_curr),cv::Scalar(0,255,0));
        cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));
        
        cv::Mat cert_grid = a1.getCertGrid();
        cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
        cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
        cv::imshow("map", img);
        cv::waitKey(WAITKEY_DELAY);


        //plot a2
        std::pair<int,int> a2_curr = a2.getCoords();
        std::cout << "Drone2 at: " << a1_curr.first << "," << a1_curr.second << std::endl;
        a2.updateCertainty(f);    
        std::pair<int,int> a2_dest = a2.determineAction();
        a2_dest = a2.moveToPosition(a2_dest);

        cv::line(map, a2.pair2Point(a2_dest), a2.pair2Point(a2_curr),cv::Scalar(0,0,255));
        cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
        cert_grid = a2.getCertGrid();
        cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
        cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
        cv::imshow("map", img);
        cv::waitKey(WAITKEY_DELAY);




    }



}