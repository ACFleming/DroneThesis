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
    Agent a1 = Agent("Drone1", 30, 30, field_x_width, field_y_length, max_range,speed, bearing);


    //RUN FUNCTION

    cv::Mat path = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);


    // std::pair<int,int> centre = {120,0};
    // std::pair<int,int> p1 = {140,140};
    // std::pair<int,int> p2 = {60,60};

    // double axes_angle = a1.angleWithHorizontal(centre, p1);

    // double arc_angle = a1.angleInsideArc(centre, p1, p2);
    // std::cout << "axes_angle: " << axes_angle << " arc_angle: " << arc_angle << std::endl;

    // cv::ellipse(path,a1.pair2Point(centre), cv::Size(a1.dist(centre,p1),a1.dist(centre,p1)), axes_angle, 0, arc_angle,cv::Scalar(255,0,255));

    // cv::imshow("Path", path);
    // cv::waitKey(0);

    std::pair<int,int> curr = a1.getCoords();
    int counter = 0;

    while(true){
        a1.measureSignalStrength(f);
        a1.updateScannedGrid();



        
    
        // cv::imshow("Frontier map", a1.getFrontierMap());
        // cv::waitKey(0);
        


        std::pair<int,int> dst = a1.determineAction();
        a1.moveToPosition(dst);
        std::pair<int,int> pos = a1.getCoords();
        std::cout << "Agent at: " << pos.first << "," << pos.second << std::endl;

        cv::line(path, cv::Point2i(curr.first, curr.second), cv::Point2i(pos.first,pos.second),cv::Scalar(255, 255-counter, counter));
        curr = pos;
        counter +=5;

    }



}