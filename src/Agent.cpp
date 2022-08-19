#include "Agent.hpp"


#define COST_VEC_PRINT

#define WAITKEY_DELAY 2

/*

    matricies work in row, col referencing.
    * - - - - - > cols
    |
    |
    |
    |
    |
    v
    rows

    open cv points work x,y but top lef origin

    * - - - - - > x
    |
    |
    |
    |
    |
    v
    y
    */

Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius, int speed, int bearing) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;
    this->field_x_width = field_width;
    this->field_y_length = field_length;
    this->scan_radius = scan_radius;
    this->speed = speed;
    this->bearing = bearing;
    this->raster_step_count = 0;
    
    


    this->certainty_grid = cv::Mat(this->field_y_length, this->field_x_width, CV_8UC1, cv::Scalar(unknown));




    // this->certainty_grid.at<uint8_t>(this->coords.second,this->coords.first) = scanned;




    



}

Agent::~Agent() {


}


std::pair<int,int> Agent::point2Pair(cv::Point2i p){
    return std::make_pair(p.x, p.y);
}

cv::Point2i Agent::pair2Point(std::pair<int,int> p){
    return cv::Point2i(p.first, p.second);
}

//clips value in range such that lower <= value < upper
int Agent::clipRange(int lower, int upper, int value){
    if(value < lower){
        value = lower;
    }else if(value >= upper){
        value = upper - 1;
    }
    return value;
}

double Agent::dist(std::pair<int,int> p1, std::pair<int,int> p2){
    return hypot(p1.first - p2.first, p1.second-p2.second);
}




cv::Mat Agent::rangeMask(int x, int y, int value){
    cv::Mat mask = cv::Mat::zeros(this->field_y_length,this->field_x_width, CV_8UC1);
    cv::Point centre(x,y);
    cv::circle(mask, centre, this->scan_radius, cv::Scalar(value,value,value), -1);
    return mask;
}




std::vector<cv::Point2i> Agent::gridSquaresInRange(int x, int y){

    std::vector<cv::Point2i> locations;

    cv::Mat mask = this->rangeMask(x, y, 255);


    cv::findNonZero(mask, locations);

    return locations;

}

std::vector<cv::Point2i> Agent::gridSquaresInRange(std::pair<int,int> coords){
    return this->gridSquaresInRange(coords.first, coords.second);
}



void Agent::updateCertainty(Field f){

    std::pair<int,int> ret = std::make_pair(-1,-1);

    //step 0 clear old measurements

    for(auto& kv_name_est: this->signal_estimations){
        Ring null_ring = Ring();
        this->signal_estimations[kv_name_est.first] = null_ring;
    }

    

    //step 1 get any signal measurements


    std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords); //to change for BT ping


    for(auto &m: measurements){

        //update signal estimation
        std::cout << m.first << ": " << m.second << std::endl; 
        Ring r =  Ring(this->field_x_width, this->field_y_length, this->coords.first, this->coords.second,m.second, 15, m.first );
        r.drawRing();
        this->signal_estimations[m.first] = r;



        if(this->signal_likelihood.count(m.first) == 0){ //if no existing likelihood region
            this->signal_likelihood[m.first] = this->certainty_grid.clone();
        }
    }
    this->measurements.push_back(measurements);



    //step 2 update likelihood

    for(auto &kv_name_ests: this->signal_estimations){
        
        
        std::cout << this->found.count(kv_name_ests.first) << std::endl;
        if(this->found.count(kv_name_ests.first) != 0){//if signal localised, skip
            continue;
        }




        cv::waitKey(0);



        if(kv_name_ests.second.isNull()){ //i.e no new info

            cv::Mat inv = this->rangeMask(this->coords.first, this->coords.second, 255);
            cv::bitwise_not(inv, inv);
            cv::imshow("inv", inv);
            cv::waitKey(WAITKEY_DELAY);
            cv::bitwise_and(inv, this->signal_likelihood[kv_name_ests.first],this->signal_likelihood[kv_name_ests.first]);

        }else{  // i.e there is new info
            //show most recent measurement ring
            cv::imshow("new measurement", this->signal_estimations[kv_name_ests.first].getCanvas());
            cv::waitKey(WAITKEY_DELAY);
            //show current likelihood region
            cv::imshow("current",this->signal_likelihood[kv_name_ests.first] );
            cv::waitKey(WAITKEY_DELAY);
            
            cv::bitwise_and(this->signal_likelihood[kv_name_ests.first], this->signal_estimations[kv_name_ests.first].getCanvas(), this->signal_likelihood[kv_name_ests.first]);
        }

        

        cv::imshow(kv_name_ests.first, this->signal_likelihood[kv_name_ests.first]);
        cv::waitKey(WAITKEY_DELAY);


        //step 2.1 find edges of the likelihood region

         

        this->signal_bounds[kv_name_ests.first] = BoundingPoints(this->signal_likelihood[kv_name_ests.first]);

 


        //GIVE THIS TO DETERMINE ACTION??

        std::cout << kv_name_ests.first << " likelihood area: " << signal_bounds[kv_name_ests.first].getArea() << std::endl;
        if(this->signal_bounds[kv_name_ests.first].getArea() < 200){ //if likelihood region smaller thatn acceptance criteria
            std::cout << "Signal found at or near: " << this->signal_bounds[kv_name_ests.first].getCentre().x  << "," << this->signal_bounds[kv_name_ests.first].getCentre().y << std::endl;
            this->found.insert(kv_name_ests.first);
        }

        // std::vector<cv::Point2i> target_loc;

        // likelihood_boundary.a



        // cv::findNonZero(this->signal_likelihood[kv_name_ests.first],target_loc);

        // 

        // cv::Point2i target_point = target_loc[random];

        // cv::Mat acceptance_criteria = this->signal_likelihood[kv_name_ests.first].clone();

        // cv::circle(acceptance_criteria, target_point, 15, cv::Scalar(0), -1);
        // cv::imshow("acceptance_criteria",acceptance_criteria);
        // cv::waitKey(WAITKEY_DELAY);



        // if(cv::countNonZero(acceptance_criteria) < 50){
        //     std::cout << "Signal found at or near: " << target_point.x  << "," << target_point.y << std::endl;
        //     this->found.insert(kv_name_ests.first);
        //     ret = std::make_pair(-1,-1); // might change this to keep looking after search is done
        // }else{
        //     ret = this->point2Pair(target_loc[random]);
        // }


        
    }





    //step 2 classify all other areas as not signals

    cv::Mat mask = this->rangeMask(this->coords.first, this->coords.second, unknown);

    cv::imshow("mask", mask);
    cv::waitKey(WAITKEY_DELAY);
    cv::subtract(this->certainty_grid, mask, this->certainty_grid);



    //step 3 get new frontiers

    cv::Mat seen;
    cv::threshold(this->certainty_grid, seen,unknown-1, unknown, cv::THRESH_BINARY_INV);

    this->frontiers = this->getImageFrontiers(seen);


    // for(auto &kv_pair: this->signal_locations){


        

    //     // cv::Mat img2;

    //     // cv::addWeighted(this->certainty_grid, 1, kv_pair.second, 1, 0, img2);
    //     // cv::imshow("Full", img2);
    //     // cv::waitKey(0);
    // }



    

}






std::vector<std::vector<cv::Point2i>> Agent::getImageFrontiers(cv::Mat frontier_img){
    std::vector<std::vector<cv::Point2i>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat approx_img = frontier_img.clone();

    
    cv::imshow("frontier_img", frontier_img);
    cv::waitKey(WAITKEY_DELAY);




    cv::findContours(frontier_img,contours, hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);
    

    if(contours.size() > 1){ //sorting by size. Could change this to check if the current point is inside or not, but may require point passing. 
        std::cout << contours.size();
        std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point2i> &a, const std::vector<cv::Point2i> &b){ return a.size() > b.size(); });
    }

    //only the largest contour remains

    




    std::vector<std::vector<cv::Point2i>> ctr_approx;
    for(auto &e: contours){
        std::vector<cv::Point2i> a;
        cv::approxPolyDP(e, a, 0.75, true);
        ctr_approx.push_back(a);
    }

    for(int i = 0; i < ctr_approx.size(); i++){
            cv::drawContours(approx_img, ctr_approx, i, cv::Scalar(255,255,255));
    }

    
    cv::imshow("OCC approx map", approx_img);
    cv::waitKey(WAITKEY_DELAY);

  
    return ctr_approx;
}

std::pair<int,int> Agent::determineAction(){


    //Rastering Assumption
    //Range of scan >= speed

    





    std::pair<int,int> next_position = this->coords;


    //dummy move

    // next_position.first += this->speed;
    // return next_position;

    

    
    std::vector<std::pair<cv::Point2i,double>> cost_vec;


    cv::Mat new_cells;
    std::vector<std::vector<cv::Point2i>> new_frontiers;

    double best_score = LONG_MIN;
    cv::Point2i best_point;





    //need to define hash function for cv::Point set

    struct point_hash {
    inline std::size_t operator()(const cv::Point2i & v) const {
        return v.x*31 + v.y;
        }
    };

    //filter frontier points based on distance

    std::unordered_set<cv::Point,point_hash> signal_frontiers;

    std::vector<cv::Point> priority_frontiers;
    std::vector<cv::Point> reserve_frontiers;
    // reserve_frontiers.



    cv::Mat priority_img = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC3);    
    
    for(auto &f_vec: this->frontiers){
        for(auto &f: f_vec){
            double dist = this->dist(this->coords, this->point2Pair(f));
            if(dist < 2 || dist > 1.5*this->speed){
                cv::circle(priority_img, f,2,cv::Scalar(0,0,255));
                reserve_frontiers.push_back(f);
            }else{
                priority_frontiers.push_back(f);
                cv::circle(priority_img, f,2,cv::Scalar(0,255,0));
            }
        }
    }


    for(auto &kv_name_bound: this->signal_bounds){
        if(this->found.count(kv_name_bound.first) == 0){//if not found
            for(auto &p: kv_name_bound.second.getBounds()){
                signal_frontiers.insert(p);
                priority_frontiers.push_back(p);
                cv::circle(priority_img, p,2,cv::Scalar(255,255,0));
            }
        }


    }


    // //identify and plug holes
    // cv::Mat inverse(new_cells.size(), CV_8UC1);
    // cv::Mat labelImage(new_cells.size(), CV_32S);


    // int connected_count = cv::connectedComponents(inverse, labelImage, 8);
    // std::vector<cv::Vec3b> colors(connected_count);
    // colors[0] = cv::Vec3b(0, 0, 0);//background
    // for(int label = 0; label < connected_count; ++label){
    //     colors[label] = cv::Vec3b( 255, 255 - 50*label, 50*label + 50 );
    // }
    
    // cv::Mat dst(new_cells.size(), CV_8UC3);
    // for(int r = 0; r < dst.rows; ++r){
    //     for(int c = 0; c < dst.cols; ++c){
    //         int label = labelImage.at<int>(r, c);
    //         cv::Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
    //         pixel = colors[label];
    //     }
    // }
    // // std::cout << "CONNECTED COUNT: " << connected_count << std::endl;
    // cv::imshow( "Connected Components", dst );
    // cv::waitKey(WAITKEY_DELAY);







    std::cout << "total frontiers: " << this->frontiers.size() << " prioritised frontiers: " << priority_frontiers.size() << std::endl;
    
    

    cv::imshow("Filtered Frontiers", priority_img);

    cv::waitKey(0);





    for(auto &f: priority_frontiers ){
#ifdef COST_VEC_PRINT
            std::cout << "Point: " << f.x << "," << f.y;
#endif
        
        cv::Mat seen;

        cv::threshold(this->certainty_grid, seen,unknown-1, unknown, cv::THRESH_BINARY_INV);

        cv::imshow("seen", seen);
        cv::waitKey(WAITKEY_DELAY);

        double score = 0;

        if(std::find(this->coord_history.begin(), this->coord_history.end(), this->point2Pair(f)) != this->coord_history.end()){
            double repeat_point_mod = -100000;
            score += repeat_point_mod;
            std::cout << " Repeat point!";
        }
        

        if(signal_frontiers.count(f) > 0 ){ //if its a signal frontier point;
            score += 10000;
        }



        double dist = this->dist(this->coords, this->point2Pair(f));
        // int(hypot(this->coords.first - f.x, this->coords.second-f.y));
        if(dist < 0.5) {
            continue;
        }

        double dist_mod = -10;
        
        // score += dist_mod * dist;
        score += dist_mod*exp(floor(dist/this->scan_radius));

#ifdef COST_VEC_PRINT
        std::cout << " Dist contribution: " << dist_mod * dist;
#endif

        cv::bitwise_or(seen, this->rangeMask(f.x, f.y, searching), new_cells);  
        cv::imshow("new_cells", new_cells);
        cv::waitKey(WAITKEY_DELAY);



        double old_scanned_count = cv::countNonZero(seen);
        double new_scanned_count = cv::countNonZero(new_cells);

        if(new_scanned_count == 0){
#ifdef COST_VEC_PRINT
            std::cout << " Lack of information gain Contribution: "<< -500 << std::endl;
#endif
            score -= 1000;
        }

        double new_scanned_ratio = (new_scanned_count-old_scanned_count)/old_scanned_count;

        double scanned_mod = 50;

        score += scanned_mod * new_scanned_ratio;
#ifdef COST_VEC_PRINT
        std::cout << " New Scanned Cells Contribution: " <<  scanned_mod  *new_scanned_ratio;
#endif




        new_frontiers = this->getImageFrontiers(new_cells);

        if(new_frontiers.size() > 0){

            double ctr_area = cv::contourArea(new_frontiers[0]);
            double ctr_perim = cv::arcLength(new_frontiers[0], true);
            std::vector<cv::Point2i> hull_points;

            cv::convexHull(new_frontiers[0], hull_points);
            double hull_area = cv::contourArea(hull_points);
            double hull_perim = cv::arcLength(hull_points, true);
            if(hull_area == 0){
                std::cout << "HUH?" << std::endl;
                hull_area = 1;
            }

            cv::Mat hull_img(new_cells.size(), CV_8UC3);
            cv::drawContours(hull_img, new_frontiers, -1, cv::Scalar(0,0,255));
            
            std::vector<std::vector<cv::Point2i>> h;
            h.push_back(hull_points);
            cv::drawContours(hull_img, h, -1, cv::Scalar(255,0,0));


            cv::imshow("hull_img", hull_img);
            cv::waitKey(WAITKEY_DELAY);

            double area_ratio = ctr_area/hull_area;
            
            double perim_ratio = ctr_perim/hull_perim;

            double area_rt_mod = 50*100;

            score += area_rt_mod * area_ratio;
#ifdef COST_VEC_PRINT
            std::cout << " Area Ratio Contribution: " << area_rt_mod * area_ratio;
#endif
            double perim_rt_mod = -0*100;

            score += perim_rt_mod * perim_ratio;
#ifdef COST_VEC_PRINT   
            std::cout << " Perim Ratio Contribution: " << perim_rt_mod * perim_ratio;
#endif

        }





//Inverse concavity check


        cv::Mat inverse(new_cells.size(), CV_8UC1);
        cv::threshold(new_cells, inverse, searching-1, 255, cv::THRESH_BINARY);
        cv::bitwise_not(inverse, inverse);
        cv::imshow("inverse", inverse );
        cv::waitKey(WAITKEY_DELAY);




        std::vector<std::vector<cv::Point>> inverse_frontiers = this->getImageFrontiers(inverse);


        if(inverse_frontiers.size() > 0){
            double inv_ctr_area = cv::contourArea(inverse_frontiers[0]);
            double inv_ctr_perim = cv::arcLength(inverse_frontiers[0], true);
            std::vector<cv::Point2i> inv_hull_points;

            cv::convexHull(inverse_frontiers[0], inv_hull_points);
            double inv_hull_area = cv::contourArea(inv_hull_points);
            double inv_hull_perim = cv::arcLength(inv_hull_points, true);
            if(inv_hull_area == 0){
                std::cout << "HUH?" << std::endl;
                inv_hull_area = 1;
            }

            cv::Mat inv_hull_img(inverse.size(), CV_8UC3);
            cv::drawContours(inv_hull_img, inverse_frontiers, -1, cv::Scalar(0,0,255));
            
            std::vector<std::vector<cv::Point2i>> inv_h;
            inv_h.push_back(inv_hull_points);
            cv::drawContours(inv_hull_img, inv_h, -1, cv::Scalar(255,0,0));


            cv::imshow("inv_hull_img", inv_hull_img);
            cv::waitKey(WAITKEY_DELAY);




            double inv_perim_ratio = inv_ctr_perim/inv_hull_perim;

            double inv_perim_rt_mod = -10*100;

            score += inv_perim_rt_mod * inv_perim_ratio;
#ifdef COST_VEC_PRINT   
            std::cout << " Perim Ratio Contribution: " << inv_perim_rt_mod * inv_perim_ratio;
#endif
        }














//         cv::Mat labelImage(new_cells.size(), CV_32S);
//         int connected_count = connectedComponents(inverse, labelImage, 8);
//         std::vector<cv::Vec3b> colors(connected_count);
//         colors[0] = cv::Vec3b(0, 0, 0);//background
//         for(int label = 0; label < connected_count; ++label){
//             colors[label] = cv::Vec3b( 255, 255 - 50*label, 50*label + 50 );
//         }
        
//         cv::Mat dst(new_cells.size(), CV_8UC3);
//         for(int r = 0; r < dst.rows; ++r){
//             for(int c = 0; c < dst.cols; ++c){
//                 int label = labelImage.at<int>(r, c);
//                 cv::Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
//                 pixel = colors[label];
//             }
//         }
//         // std::cout << "CONNECTED COUNT: " << connected_count << std::endl;
//         cv::imshow( "Connected Components", dst );
//         cv::waitKey(WAITKEY_DELAY);

//         double connected_mod = 0*-50; 


//         score += connected_mod * connected_count;
// #ifdef COST_VEC_PRINT
//         std::cout << " Connected Components Penalty: " << connected_mod * connected_count;
// #endif







#ifdef COST_VEC_PRINT
        std::cout  << " Final Score: " << score <<  std::endl;  
#endif      
        if(score > best_score){
            best_score = score;
            best_point = f;
        }


        cv::waitKey(0);
        
    }

    if(best_score < -90000){ //repeat point
        std::cout << "SHOULD BE DONE!" << std::endl;
        std::vector<cv::Point2i> missed_spots;

        cv::findNonZero(this->certainty_grid,missed_spots);

        if(missed_spots.size() == 0){ //fully explored
            std::cout <<"DONE!" << std::endl;
            cv::waitKey(0);
            exit(0);
        }//else find 

        double missed_p_dist = this->field_x_width*this->field_x_width + this->field_y_length*this->field_y_length;
        for(auto &p: missed_spots){
            if(missed_p_dist > this->dist(this->coords, this->point2Pair(p))){
                best_point = p;
                missed_p_dist = this->dist(this->coords, this->point2Pair(p));
            }
        }

        

    }




    // //Finf min arg
    // std::pair<cv::Point2i,double>result = *std::max_element(cost_vec.cbegin(), cost_vec.cend(), [](const std::pair<cv::Point2i,double> &lhs, const std::pair<cv::Point2i,double> &rhs) {
    //     return lhs.second < rhs.second;    
    // });


    std::cout <<  "BEST! " << "Point:" << best_point.x <<"," << best_point.y << " Score: " << best_score << std::endl;
    cv::Mat best = this->certainty_grid.clone();
    cv::circle(best, best_point, 3, cv::Scalar(255));
    cv::imshow("best", best);
    cv::waitKey(WAITKEY_DELAY);


    return this->point2Pair(best_point);


}

std::pair<int,int> Agent::moveToPosition(std::pair<int,int> pos){
    // this->certainty_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
    pos.first = this->clipRange(0, this->field_x_width, pos.first);
    pos.second = this->clipRange(0, this->field_y_length, pos.second);

    std::pair<int,int> interpolated_pos = pos;

    double factor = this->speed/this->dist(pos, this->coords);
    


    
    if(factor < 1){
        interpolated_pos.first = (pos.first-this->coords.first)*factor + this->coords.first;
        interpolated_pos.second = (pos.second-this->coords.second)*factor + this->coords.second;
    }else{
        interpolated_pos.first = pos.first;
        interpolated_pos.second = pos.second;
    }

    

    std::cout << "Interpolation factor: " << factor << " Interpolated point: " << interpolated_pos.first << "," << interpolated_pos.second << std::endl;


    interpolated_pos.first = this->clipRange(0, this->field_x_width, interpolated_pos.first);
    interpolated_pos.second = this->clipRange(0, this->field_y_length, interpolated_pos.second);



    this->coords = interpolated_pos;
    // this->certainty_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
    this->coord_history.push_back(this->coords);

    return interpolated_pos;
}



// void Agent::measureSignalStrength(Field f) {
//     std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords);
//     std::vector<Ring> curr_est;
//     for(auto &m: measurements){
//         std::cout << m.first << ": " << m.second << std::endl;
        
        
//         Ring r = Ring(this->field_x_width, this->field_y_length, this->coords.first, this->coords.second,m.second, 1, m.first ); 
//         r.drawRing();
//         cv::Mat img = r.getCanvas();
//         cv::imshow("New Measurement", img);
//         cv::waitKey(WAITKEY_DELAY);
        
//         if(this->signal_estimations[r.getName()].size() == 0){
//             this->signal_estimations[r.getName()] = curr_est;
//         }
//         this->signal_estimations[r.getName()].push_back(r);


//     }
//     this->measurements.push_back(measurements);

//     for(auto &key_val: this->signal_estimations){
//         cv::Mat img = Ring::intersectRings(key_val.second);
//         cv::imshow(key_val.first, img);
//         cv::waitKey(WAITKEY_DELAY);
//     }

    
// }





std::pair<int,int> Agent::getCoords(){
    return this->coords;
}

std::string Agent::logAgent() {
    std::ofstream agent_log;
    std::string file_path = "logs/" + this->name + "_agent_log.csv";
    agent_log.open(file_path);
    agent_log << "drone_x, drone_y";
    //this->measurements[0] is the number of sources
    for(int i = 0; i < this->measurements[0].size() ; i++){
        agent_log << ",id" << i << ",dist" << i;
    }
    agent_log << std::endl;
    
    //For each timestep

    auto it_coords = this->coord_history.begin();
    auto it_measurements = this->measurements.begin();
    while(it_coords != this->coord_history.end()  && it_measurements != this->measurements.end()){
        
        agent_log << (*it_coords).first << "," << (*it_coords).second;  
        for(auto &measurement: (*it_measurements)){
            agent_log << "," << measurement.first << "," << measurement.second;
        }
        agent_log << std::endl;
        it_coords++;
        it_measurements++;
        
    }
    
    return file_path;

}




cv::Mat Agent::getCertGrid(){
    return this->certainty_grid;
}

cv::Mat Agent::getFrontierMap(){
    cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);



    cv::drawContours(frontier_map, this->frontiers  , -1, cv::Scalar(255));
    
    return frontier_map;
}