#include "Field.hpp"
#include "Agent.hpp"
#include "Ring.hpp"
#include "Grid.hpp"
#include <opencv2/core.hpp>
#include <map>
#include <string>



int main (int argc, char* argv[]){


        // cv::Mat field_shape = cv::Mat(300, 300, CV_8UC1, cv::Scalar(empty));
        // cv::imshow("Field shape", field_shape);
        // cv::waitKey(0);
        // cv::Point2i p1 = cv::Point2i(50,50);
        // cv::Point2i p2 = cv::Point2i(100,250);
        // cv::Point2i p3 = cv::Point2i(200,250);
        // cv::Point2i p4 = cv::Point2i(250,50);
        // std::vector<cv::Point2i> boundary;
        // boundary.push_back(p1);
        // boundary.push_back(p2);
        // boundary.push_back(p3);
        // boundary.push_back(p4);
        // for(int i = 0; i < 4; i++){
        //     cv::line(field_shape, boundary[i],boundary[(i+1)%4], cv::Scalar(occupied));
        // }
        

        

        // cv::imshow("Field shape", field_shape);
        // cv::waitKey(0);
        
        // cv::fillConvexPoly(field_shape, boundary, cv::Scalar(searching));
        // for(int i = 0; i < 4; i++){
        //     cv::line(field_shape, boundary[i],boundary[(i+1)%4], cv::Scalar(occupied));
        // }
        
        // cv::imshow("Field shape", field_shape);
        // cv::waitKey(0);


        // std:: cout << cv::pointPolygonTest(boundary, cv::Point2i(200,50), false) << std::endl;



    std::string number_of_agents = std::string("single/");
    std::string type_of_test = std::string("full");


    int num_tests = 100;
    int rand_seed_start = 0;
    int source_start = 1;
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

                int field_x_rows = 300;
                int field_y_cols = 300;
                int num_sources = source_count;
                double std_dev_noise = 3;
                double max_range = 20;
                double speed = 20;
                srand((unsigned int)(test));
                std::map<std::string, Grid> certainty_grids = std::map<std::string, Grid>();

                Field f = Field(field_x_rows,field_y_cols,num_sources, std_dev_noise, max_range);  
                std::cout << "SHOWING Agents" << std::endl;
                Agent a1 = Agent("Drone1", 15 ,0, field_x_rows, field_y_cols, max_range, std_dev_noise,speed,  &certainty_grids);
                Agent a2 = Agent("Drone2", 0 , 15, field_x_rows, field_y_cols, max_range,std_dev_noise, speed, &certainty_grids);
                // Agent a3 = Agent("Drone2", 299 ,0, field_x_rows, field_y_cols, max_range,speed, &certainty_grids);
                Agent::step_counter = 0;
                // a1.output = &output_agent;
                // a2.output = &output_agent;
                // a3.output = &output_agent;


                // RUN FUNCTION

                cv::Mat map = cv::Mat::zeros(field_y_cols, field_x_rows, CV_8UC3);
                cv::Mat img = cv::Mat::zeros(field_y_cols, field_x_rows, CV_8UC3);
                cv::Mat locations = cv::Mat::zeros(field_y_cols, field_x_rows, CV_8UC3);
                cv::Mat img2 = cv::Mat::zeros(field_y_cols, field_x_rows, CV_8UC3);
                
                //Display true location
                for(auto &c: f.getSources()){
                    cv::circle(map, cv::Point2i(c.getCoords().first, c.getCoords().second), 3, cv::Scalar(255,0,255) );
                    cv::circle(locations, cv::Point2i(c.getCoords().first, c.getCoords().second), 2, cv::Scalar(255,255,255 ));
                }

#ifdef SHOW_IMG
                std::pair<int,int> a1_curr = a1.getCoords();    
                cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));  

                // std::pair<int,int> a2_curr = a2.getCoords();
                // cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
                    
                cv::imshow("map", map);
                cv::waitKey(WAITKEY_DELAY);
#endif
                a1.updateCertainty(f);
                // a2.updateCertainty(f); 

                while(true){
                    // cv::waitKey(0);
                    //plot a1 


                    std::pair<int,int> a1_curr = a1.getCoords();
                    std::cout << "Drone 1 at: " << a1_curr.first << "," << a1_curr.second << std::endl;
                    std::cout << "Step: " << Agent::step_counter << std::endl;
                       
                    std::pair<int,int> a1_dest = a1.determineAction();
                    if(a1_dest.first == -1 ){ //exploration complete
                        break;
                    }
                    a1_dest = a1.moveToPosition(a1_dest);
                    cv::line(map, a1.pair2Point(a1_dest), a1.pair2Point(a1_curr),cv::Scalar(0,255,0));
                    cv::circle(map, a1.pair2Point(a1_curr),2,cv::Scalar(0,255,0));
                    cv::Mat cert_grid = a1.getMap();
                    cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
                    cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
// #ifdef SHOW_IMG
                    cv::imshow("map", img);
                    cv::waitKey(WAITKEY_DELAY);
                    cv::bitwise_or(locations, a1.getSignalLocations(), img2);
                    cv::imshow("locations", img2);
                    cv::waitKey(WAITKEY_DELAY);
// #endif



                    // a1.updateMap();
                    a1.updateCertainty(f); 



                    //plot a2
                    std::pair<int,int> a2_curr = a2.getCoords();
                    std::cout << "Drone 2 at: " << a2_curr.first << "," << a2_curr.second << std::endl;
                    std::cout << "Step: " << Agent::step_counter << std::endl;
                     
                    std::pair<int,int> a2_dest = a2.determineAction();
                    if(a2_dest.first == -1 ){ //exploration complete
                        break;
                    }
                    a2_dest = a2.moveToPosition(a2_dest);
                    cv::line(map, a2.pair2Point(a2_dest), a2.pair2Point(a2_curr),cv::Scalar(0,0,255));
                    cv::circle(map, a2.pair2Point(a2_curr),2,cv::Scalar(0,0,255));
                    cert_grid = a2.getMap();
                    cv::cvtColor(cert_grid, cert_grid, cv::COLOR_GRAY2BGR);
                    cv::addWeighted(map, 1, cert_grid, 0.5, 0, img);
// #ifdef SHOW_IMG
                    cv::imshow("map", img);
                    cv::waitKey(WAITKEY_DELAY);
                    cv::bitwise_or(locations, a1.getSignalLocations(), img2);
                    cv::imshow("locations", img2);
                    cv::waitKey(WAITKEY_DELAY);
// #endif

                    // a2.updateMap();
                    a2.updateCertainty(f);   



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
                for(auto &s: f.getSources()){
                    a1.verifySignalLocations(s.getId(), s.getCoords());
                }
                f.logField(&output_field);
            
                cv::imwrite(file_path_map, map);
                cv::imwrite(file_path_locations, a1.getSignalLocations());
                output_agent.close();
                output_field.close();
        }
    }







}