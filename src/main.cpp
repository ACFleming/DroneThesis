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
    Agent a1 = Agent("Drone1", 0 , 50, field_x_width, field_y_length, max_range,speed, bearing);
    // Agent a2 = Agent("Drone2", 50, 0, field_x_width, field_y_length, max_range,speed, bearing);

    

    // RUN FUNCTION

    cv::Mat map = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);



    //Display true location
    for(auto &c: f.getSources()){
        cv::circle(map, cv::Point2i(c.getCoords().first, c.getCoords().second), 3, cv::Scalar(255,0,255) );
    }




    cv::imshow("map", map);
    cv::waitKey(0);




    int counter = 0;

    while(true){

        

        //plot a1 

        std::pair<int,int> a1_curr = a1.getCoords();

        cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));

        //plot a2
        // cv::Point2i a2_curr = a2.pair2Point(a1.getCoords());

        // cv::circle(map, a2_curr,2,cv::Scalar(0,0,255));

        a1.updateCertainty(f);



        
        std::cout << "Agent at: " << a1_curr.first << "," << a1_curr.second << std::endl;




        // cv::imshow("Certainty grid", cert_grid);
        // cv::waitKey(0);





        // curr = pos;
        counter +=1;
        cv::Mat cert_grid = a1.getCertGrid();
        cv::Mat img = cv::Mat(cert_grid.size(), CV_8UC3);
        cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);

        cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
        // cv::bitwise_or(map, cert_grid, img);

        cv::imshow("map", img);
        cv::waitKey(10);
    


        
    
        // cv::imshow("Frontier map", a1.getFrontierMap());
        // cv::waitKey(0);

        std::pair<int,int> a1_dest = a1.determineAction();

        a1_dest = a1.moveToPosition(a1_dest);



        cv::line(map, a1.pair2Point(a1_dest), a1.pair2Point(a1_curr),cv::Scalar(255, 255-counter, counter));





    }



}