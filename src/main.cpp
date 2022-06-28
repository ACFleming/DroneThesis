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
    int speed = 60;
    int bearing = 90;


    // std::cout << cv::getBuildInformation() << std::endl;

    Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
    Agent a1 = Agent("Drone1", 0, 0, field_x_width, field_y_length, max_range,speed, bearing);

    std::vector<cv::Point2i> ret = a1.getOctagonPoints(200,100);

    for(auto &p: ret){
        std::cout << p.x << "," << p.y << std::endl;
    }

    std::cout << "Area:" <<  cv::contourArea(ret) << std::endl;


    //rows = 600, cols = 900
    cv::Mat img = cv::Mat::zeros(600,900,CV_8UC1);

    
    std::vector<std::vector<cv::Point2i>> ctrs;

    // std::vector<cv::Point2i> points;
    // points.push_back(cv::Point2i(10,10));
    // points.push_back(cv::Point2i(100,10));
    // points.push_back(cv::Point2i(100,100));
    // points.push_back(cv::Point2i(10,100));


    ctrs.push_back(ret);

    cv::Scalar color( 255, 0,255);

    cv::drawContours(img, ctrs, -1,color );



    cv::imshow("IMAGE", img);
    cv::waitKey(0);

    // Agent a2 = Agent("Drone2", 0.75*field_x_width, 0.75*field_y_length, field_x_width, field_y_length);
    
    // std::cout << "Showing grid" << std::endl;

    // // std::vector<cv::Point> locations =  a1.getAllGridSquares(a1.getCoords().first, a1.getCoords().second);

 



    
    // while(true){
    //     a1.measureSignalStrength(f);

    //     cv::imshow("Occ Grid", a1.getOccGrid());
    //     cv::waitKey(10);

    //     a1.globalFrontiers();




    //     std::pair<int,int> action = a1.determineAction();
    //     std::cout << action.first << "," << action.second << std::endl;
    //     if(action.first == -1 && action.second == -1) break;
    //     a1.moveToPosition(action);
    
        

        

    // }
    
    



    // std::string a1_file_path = a1.logAgent();
    // // std::string a2_file_path = a2.logAgent();
    // std::string field_file_path = f.logField();

    // std::ofstream file_path_log;
    // file_path_log.open("logs/file_path_log.csv");
    // file_path_log << "File Paths" << std::endl;
    // file_path_log << field_file_path << std::endl;
    // file_path_log << a1_file_path << std::endl;
    // // file_path_log << a2_file_path << std::endl;
    // file_path_log.close();



}