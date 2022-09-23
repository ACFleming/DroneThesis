#include "Field.hpp"
#include "Agent.hpp"
#include "Ring.hpp"
#include "Grid.hpp"
#include <opencv2/core.hpp>
#include <map>
#include <string>
// #include <filesystem>


int main (int argc, char* argv[]){
    //seed the random number generator
    
    // std::ofstream output_test;
    // std::string file_path = "here.txt";
    // output_test.open(file_path);
    // output_test << "TEST";
    // output_test.close();
    // std::cout << std::filesystem::current_path() << std::endl << std::endl;
    
    std::string number_of_agents = std::string("single/");
    std::string type_of_test = std::string("dist_seen_perim_area");


    int num_tests = 100;
    int rand_seed_start = 0;
    int source_start = 0;
    int max_sources = 5;
    for(int test = rand_seed_start; test < rand_seed_start + num_tests; test ++){
        for (int source_count = source_start; source_count < max_sources; source_count++){
                std::cout << "Seed: " << test << " Num sources: " << source_count << " ***********************" << std::endl;
                std::ofstream output_field;
                std::string file_path_field =       number_of_agents + type_of_test + std::string("/logs/field_log")    + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".csv";
                output_field.open(file_path_field);
                std::ofstream output_agent;
                std::string file_path_agent =       number_of_agents + type_of_test + std::string("/logs/agent_log")    + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".csv";
                output_agent.open(file_path_agent);
                std::string file_path_map =         number_of_agents + type_of_test + std::string("/images/map")        + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".png";
                std::string file_path_locations =   number_of_agents + type_of_test + std::string("/images/locations")  + std::string("_test_") + std::to_string(test) + "_sc_" + std::to_string(source_count) + ".png";

                int field_x_width = 400;
                int field_y_length = 400;
                int num_sources = source_count;
                int std_dev_noise = 3;
                int max_range = 20; 
                int speed = 20;
                srand((unsigned int)(test));
                std::map<std::string, Grid> certainty_grids = std::map<std::string, Grid>();

                Field f = Field(field_x_width,field_y_length,num_sources, std_dev_noise, max_range);  
                Agent a1 = Agent("Drone1", 0 ,20, field_x_width, field_y_length, max_range,speed, &certainty_grids);
                // Agent a2 = Agent("Drone2", 20 , 0, field_x_width, field_y_length, max_range,speed, &certainty_grids);
                // Agent a3 = Agent("Drone2", 299 ,0, field_x_width, field_y_length, max_range,speed, &certainty_grids);
                Agent::step_counter = 0;
                // a1.output = &output_agent;
                // a2.output = &output_agent;
                // a3.output = &output_agent;


                // RUN FUNCTION

                cv::Mat map = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);
                cv::Mat img = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);
                cv::Mat locations = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);
                cv::Mat img2 = cv::Mat::zeros(field_y_length, field_x_width, CV_8UC3);

                //Display true location
                for(auto &c: f.getSources()){
                    cv::circle(map, cv::Point2i(c.getCoords().first, c.getCoords().second), 3, cv::Scalar(255,0,255) );
                    cv::circle(locations, cv::Point2i(c.getCoords().first, c.getCoords().second), 2, cv::Scalar(255,255,255 ), -1);
                }

#ifdef SHOW_IMG
                std::pair<int,int> a1_curr = a1.getCoords();    
                cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));  

                // std::pair<int,int> a2_curr = a2.getCoords();
                // cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
                    
                cv::imshow("map", map);
                cv::waitKey(1);
#endif
            

                while(true){
                    // cv::waitKey(0);
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
// #ifdef SHOW_IMG
                    cv::imshow("map", img);
                    cv::waitKey(1);
                    cv::bitwise_or(locations, a1.getSignalLocations(), img2);
                    cv::imshow("locations", img2);
                    cv::waitKey(1);
// #endif
//                     //plot a2
//                     std::pair<int,int> a2_curr = a2.getCoords();
//                     std::cout << "Drone 2 at: " << a2_curr.first << "," << a2_curr.second << std::endl;
//                     a2.updateCertainty(f);    
//                     std::pair<int,int> a2_dest = a2.determineAction();
//                     if(a2_dest.first == -1 ){ //exploration complete
//                         break;
//                     }
//                     a2_dest = a2.moveToPosition(a2_dest);
//                     a2.updateMap();

//                     cv::line(map, a2.pair2Point(a2_dest), a2.pair2Point(a2_curr),cv::Scalar(0,0,255));
//                     cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
//                     cert_grid = a2.getMap();
//                     cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
//                     cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
// // #ifdef SHOW_IMG
//                     cv::imshow("map", img);
//                     cv::waitKey(1);
//                     cv::imshow("locations", a2.getSignalLocations());
//                     cv::waitKey(1);
// // #endif

                    // //plot a3
                    // std::pair<int,int> a3_curr = a3.getCoords();
                    // std::cout << "Drone 3 at: " << a3_curr.first << "," << a3_curr.second << std::endl;
                    // a3.updateCertainty(f);    
                    // std::pair<int,int> a3_dest = a3.determineAction();
                    // if(a3_dest.first == -1 ){ //exploration complete
                    //     break;
                    // }
                    // a3_dest = a3.moveToPosition(a3_dest);
                    // a3.updateMap();

                    // cv::line(map, a3.pair2Point(a3_dest), a3.pair2Point(a3_curr),cv::Scalar(255,0,0));
                    // cv::circle(map, a3.pair2Point(a3_curr),2,cv::Scalar(255,0,0));
                    // cert_grid = a3.getMap();
                    // cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
                    // cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
#ifdef SHOW_IMG
                    // // cv::imshow("map", img);
                    // // cv::waitKey(1);
                    // // cv::imshow("locations", a2.getSignalLocations());
                    // // cv::waitKey(1);
#endif

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