#include "Field.hpp"
#include "Agent.hpp"
#include "Ring.hpp"
#include "Grid.hpp"
#include <opencv2/core.hpp>
#include <map>
#include <string>
#include <filesystem>


int main (int argc, char* argv[]){
    //seed the random number generator
    
    // std::ofstream output_test;
    // std::string file_path = "here.txt";
    // output_test.open(file_path);
    // output_test << "TEST";
    // output_test.close();
    // std::cout << std::filesystem::current_path() << std::endl << std::endl;
    


    int num_tests = 100;
    int rand_seed_start = 0;
    int max_sources = 5;
    for(int test = rand_seed_start; test < rand_seed_start + num_tests; test ++){
        for (int source_count = 0; source_count < max_sources; source_count++){

                std::ofstream output_field;
                std::string file_path_field = std::string("single") + std::string("_agent_logs/field_log") + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".csv";
                output_field.open(file_path_field);
                std::ofstream output_agent;
                std::string file_path_agent = std::string("single") + std::string("_agent_logs/agent_log") + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".csv";
                output_agent.open(file_path_agent);

                std::string file_path_map = std::string("single") + std::string("_agent_images/map") + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".png";
                std::string file_path_locations = std::string("single") + std::string("_agent_images/locations") + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".png";

                int field_x_width = 300;
                int field_y_length = 300;
                int num_sources = source_count;
                int std_dev_noise = 4;
                int max_range = 20; 
                int speed = 20;
                srand((unsigned int)(test));
                std::map<std::string, Grid> certainty_grids = std::map<std::string, Grid>();

                Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
                Agent a1 = Agent("Drone1", 0 ,20, field_x_width, field_y_length, max_range,speed, &certainty_grids);
                // Agent a2 = Agent("Drone2", 20 ,0, field_x_width, field_y_length, max_range,speed, &certainty_grids);
                Agent::step_counter = 0;
                a1.output = &output_agent;
                // a2.output = &output_agent;


                // RUN FUNCTION

                cv::Mat map = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);
                cv::Mat img = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);

                //Display true location
                for(auto &c: f.getSources()){
                    cv::circle(map, cv::Point2i(c.getCoords().first, c.getCoords().second), 3, cv::Scalar(255,0,255) );
                }

                // std::pair<int,int> a1_curr = a1.getCoords();    
                // cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));  

                // std::pair<int,int> a2_curr = a2.getCoords();
                // cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
                    
                // cv::imshow("map", map);
                // cv::waitKey(1);

            

                while(true){
                    //plot a1 
                    std::pair<int,int> a1_curr = a1.getCoords();
                    std::cout << "Drone 1 at: " << a1_curr.first << "," << a1_curr.second << std::endl;
                    a1.updateCertainty(f);    
                    std::pair<int,int> a1_dest = a1.determineAction();
                    if(a1_dest.first == -1 ){ //exploration complete
                        break;
                    }
                    a1_dest = a1.moveToPosition(a1_dest);
                    a1.updateMap();


                    cv::line(map, a1.pair2Point(a1_dest), a1.pair2Point(a1_curr),cv::Scalar(0,255,0));
                    cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));
                    cv::Mat cert_grid = a1.getMap();
                    cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
                    cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
                    // cv::imshow("map", img);
                    // cv::waitKey(1);
                    // cv::imshow("locations", a1.getSignalLocations());
                    // cv::waitKey(1);

                    // //plot a2
                    // std::pair<int,int> a2_curr = a2.getCoords();
                    // std::cout << "Drone 2 at: " << a1_curr.first << "," << a1_curr.second << std::endl;
                    // a2.updateCertainty(f);    
                    // std::pair<int,int> a2_dest = a2.determineAction();
                    // if(a2_dest.first == -1 ){ //exploration complete
                    //     break;
                    // }
                    // a2_dest = a2.moveToPosition(a2_dest);
                    // a2.updateMap();

                    // cv::line(map, a2.pair2Point(a2_dest), a2.pair2Point(a2_curr),cv::Scalar(0,0,255));
                    // cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
                    // cert_grid = a2.getMap();
                    // cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
                    // cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
                    // // cv::imshow("map", img);
                    // // cv::waitKey(1);
                    // // cv::imshow("locations", a2.getSignalLocations());
                    // // cv::waitKey(1);

                }
                a1.logAgent();
                f.logField(&output_field);
                
                cv::imwrite(file_path_map, map);
                cv::imwrite(file_path_locations, a1.getSignalLocations());
                output_agent.close();
                output_field.close();
        }
    }







}