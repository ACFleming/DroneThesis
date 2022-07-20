#include "Agent.hpp"


#define COST_VEC_PRINT

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


double Agent::angleInsideArc(std::pair<int,int> c, std::pair<int,int> p1, std::pair<int,int> p2){
    double radius = this->dist(c,p1);
    double dist = this->dist(p1,p2);
    if(dist == 0) return 0.0;
    double rad =  acos(1-dist*dist/(2*radius*radius));
    return rad*180/PI;
}

double Agent::angleWithHorizontal(std::pair<int,int>c, std::pair<int,int> p){
    double radius = dist(c,p);
    double angle = angleInsideArc(c, std::make_pair(c.first+radius, c.second), p);
    if(p.second < c.second) angle = -1*angle;
    return angle;
}

void Agent::drawArc(cv::Mat img, std::pair<int,int> c, std::pair<int,int> p1, std::pair<int,int> p2){




    double axes_angle = this->angleWithHorizontal(c, p1);

    double arc_angle = this->angleInsideArc(c, p1, p2);
    std::cout << "axes_angle: " << axes_angle << " arc_angle: " << arc_angle << std::endl;

    cv::ellipse(img,this->pair2Point(c), cv::Size(this->dist(c,p1),this->dist(c,p1)), axes_angle, 0, arc_angle,cv::Scalar(255,0,255));

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



std::pair<int,int> Agent::updateCertainty(Field f){

    std::pair<int,int> ret = std::make_pair(-1,-1);

    std::map<std::string, bool> has_new_measurement;

    // for(auto &kv_pair: this->has_new_measurement){
    //     kv_pair.second = false;

    // }

    

    //step 1 get any signal measurements


    std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords);


    for(auto &m: measurements){
        std::cout << m.first << ": " << m.second << std::endl;
        has_new_measurement[m.first] = true;


        



        
        if(this->signal_estimations[m.first].size() == 0){
            std::vector<Ring> empty_ring_vec;
            this->signal_estimations[m.first] = empty_ring_vec;
            this->signal_locations[m.first] = this->certainty_grid.clone();
            
            
        }
        
        Ring r =  Ring(this->field_x_width, this->field_y_length, this->coords.first, this->coords.second,m.second, 6*3, m.first );
        r.drawRing();
        this->signal_estimations[m.first].push_back(r);



    }
    this->measurements.push_back(measurements);



    // for(auto &kv_pair: this->signal_estimations){
        
    //     this->signal_locations[kv_pair.first] = Ring::intersectRings(this->signal_estimations[kv_pair.first]);
    //     cv::imshow(kv_pair.first, this->signal_locations[kv_pair.first]);
    //     cv::waitKey(0);
    //     std::vector<cv::Point2i> target_loc;
    //     cv::findNonZero(this->signal_locations[kv_pair.first],target_loc);

    //     int random = rand() % target_loc.size();
    //     ret = this->point2Pair(target_loc[random]);


    // }   






    for(auto &kv_pair: this->signal_estimations){
        





        if(has_new_measurement[kv_pair.first]){ // i.e there is new info
            

            cv::imshow("new measurement", this->signal_estimations[kv_pair.first].back().getCanvas());
            cv::waitKey(0);

            cv::imshow("current",this->signal_locations[kv_pair.first] );
            cv::waitKey(0);


            cv::bitwise_and(this->signal_locations[kv_pair.first], this->signal_estimations[kv_pair.first].back().getCanvas(), this->signal_locations[kv_pair.first]);
        }else{  //i.e this new scan of nothing is new info
            cv::Mat inv = this->rangeMask(this->coords.first, this->coords.second, target);
            cv::bitwise_not(inv, inv);
            cv::imshow("inv", inv);
            cv::waitKey(0);
            cv::bitwise_and(inv, this->signal_locations[kv_pair.first],this->signal_locations[kv_pair.first]);
        }

        

        cv::imshow("after update", this->signal_locations[kv_pair.first]);
        cv::waitKey(0);

        std::vector<cv::Point2i> target_loc;

        cv::findNonZero(this->signal_locations[kv_pair.first],target_loc);

        int random = rand() % target_loc.size();

        cv::Point2i target_point = target_loc[random];

        cv::Mat acceptance_criteria = this->signal_locations[kv_pair.first].clone();

        cv::circle(acceptance_criteria, target_point, 15, cv::Scalar(0), -1);
        cv::imshow("acceptance_criteria",acceptance_criteria);
        cv::waitKey(0);
        if(cv::countNonZero(acceptance_criteria) < 50){
            std::cout << "Signal found at or near: " << target_point.x  << "," << target_point.y << std::endl;
            this->is_found[kv_pair.first] = true;
            ret = std::make_pair(-1,-1); // might change this to keep looking after search is done
        }else{
            ret = this->point2Pair(target_loc[random]);
        }


        
    }





    //step 2 classify all other areas as not signals

    cv::Mat mask = this->rangeMask(this->coords.first, this->coords.second, unknown);

    cv::imshow("mask", mask);
    cv::waitKey(0);
    cv::subtract(this->certainty_grid, mask, this->certainty_grid);



    //step 3 get new frontiers


    this->frontiers = this->getNewFrontiers(this->coords.first, this->coords.second);


    // for(auto &kv_pair: this->signal_locations){


        

    //     // cv::Mat img2;

    //     // cv::addWeighted(this->certainty_grid, 1, kv_pair.second, 1, 0, img2);
    //     // cv::imshow("Full", img2);
    //     // cv::waitKey(0);
    // }

    return ret;

    

}



std::vector<std::vector<cv::Point2i>> Agent::getNewFrontiers(int x, int y){


    
    cv::Mat frontier_img = this->certainty_grid.clone();
    cv::Mat mask = this->rangeMask(x, y, empty);
    cv::bitwise_or(frontier_img, mask, frontier_img);


    cv::imshow("New frontiers", frontier_img);
    cv::waitKey(0);



    std::vector<std::vector<cv::Point2i>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::threshold(frontier_img, frontier_img, unknown-1, unknown, cv::THRESH_BINARY_INV);

    cv::findContours(frontier_img,contours, hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);

    if(contours.size() > 1){
        std::cout << contours.size();
        std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point2i> &a, const std::vector<cv::Point2i> &b){ return a.size() > b.size(); });
    }


    




    std::vector<std::vector<cv::Point2i>> ctr_approx;
    for(auto &e: contours){
        std::vector<cv::Point2i> a;
        cv::approxPolyDP(e, a, 1, true);
        ctr_approx.push_back(a);
    }

    for(int i = 0; i < ctr_approx.size(); i++){
            cv::drawContours(frontier_img, ctr_approx, i, cv::Scalar(255,255,255));
    }

    
    cv::imshow("OCC approx map", frontier_img);
    cv::waitKey(0);





    

    
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

    int curr_scanned = cv::countNonZero(this->certainty_grid);
    // int curr_frontier_count = this->frontiers[0].size();

    // std::vector<cv::Point2i> frontiers = this->getNewFrontiers(this->coords.first, this);
    cv::Mat new_cells;
    std::vector<std::vector<cv::Point2i>> new_frontiers;

    double best_score = INT16_MIN;
    cv::Point2i best_point;

    // std::cout << "Frontier chain count: " << this->frontiers.size() << std::endl;    
    // for(auto &v: this->frontiers){
    //     std::cout << "Frontier size: " << v.size() << std::endl;
    // }


    for(auto &f: this->frontiers[0] ){
#ifdef COST_VEC_PRINT
            std::cout << "Point: " << f.x << "," << f.y;
#endif
        double score = 0;

        if(std::find(this->coord_history.begin(), this->coord_history.end(), this->point2Pair(f)) != this->coord_history.end()){
            double repeat_point_mod = -10000;
            score += repeat_point_mod;
            std::cout << " Repeat point!";
        }
        




        int dist = int(hypot(this->coords.first - f.x, this->coords.second-f.y));
        if(dist == 0) continue;

        
        double dist_mod = -1.0;
        
        score += dist_mod * dist;

#ifdef COST_VEC_PRINT
        std::cout << " Dist contribution: " << dist_mod * dist;
#endif

        cv::bitwise_or(this->certainty_grid, this->rangeMask(f.x, f.y, 200), new_cells);  
        cv::imshow("new_cells", new_cells);
        cv::waitKey(0);


        int new_scanned_count = cv::countNonZero(new_cells) - curr_scanned;

        if(new_scanned_count == 0){
#ifdef COST_VEC_PRINT
            std::cout << "No information gain here!"<< std::endl;
#endif
            score -= 500;
        }

        double new_scanned_ratio = new_scanned_count/(PI*this->scan_radius*this->scan_radius);

        double scanned_mod = 0.0;

        score += scanned_mod * new_scanned_ratio;
#ifdef COST_VEC_PRINT
        std::cout << " New Scanned Cells Contribution: " <<  scanned_mod  *new_scanned_count;
#endif
        // cv::imshow("New cells", new_cells);
        // cv::waitKey(0);



        new_frontiers = this->getNewFrontiers(f.x, f.y);


        cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);

        cv::drawContours(frontier_map, new_frontiers, -1, cv::Scalar(255));


        double ctr_area = cv::contourArea(new_frontiers[0]);
        double ctr_perim = cv::arcLength(new_frontiers[0], true);
        std::vector<cv::Point2i> hull_points;

        cv::convexHull(new_frontiers[0], hull_points);
        double hull_area = cv::contourArea(hull_points);
        double hull_perim = cv::arcLength(hull_points, true);
        if(hull_area == 0){
            std::cout << "HUH?" << std::endl;
            hull_area = cv::contourArea(hull_points);
        }

        cv::Mat hull_img = new_cells;
        std::vector<std::vector<cv::Point2i>> h;
        h.push_back(hull_points);
        cv::drawContours(hull_img, h, -1, cv::Scalar(255));

        cv::imshow("hull_img", hull_img);
        cv::waitKey(0);

        double area_ratio = ctr_area/hull_area;
        
        double perim_ratio = ctr_perim/hull_perim;

        double area_rt_mod = 50*100;

        score += area_rt_mod * area_ratio;
#ifdef COST_VEC_PRINT
        std::cout << " Area Ratio Contribution: " << area_rt_mod * area_ratio;
#endif
        // double perim_rt_mod = -50*100;

        // score += perim_rt_mod * perim_ratio;
            
        // std::cout << " Perim Ratio Contribution: " << perim_rt_mod * perim_ratio;




  
        // cv::Mat cc; 
        // int num_connected = cv::connectedComponents(this->certainty_grid, cc);

        // cv::imshow("OCC GRID", this->certainty_grid);
        // cv::waitKey(0);

    

        cv::Mat inverse(new_cells.size(), CV_8UC1);
        cv::Mat labelImage(new_cells.size(), CV_32S);
        cv::bitwise_not(this->certainty_grid, inverse);
        cv::threshold(inverse, inverse, target, 255, cv::THRESH_BINARY);

        cv::bitwise_or(inverse, this->rangeMask(f.x, f.y, 255), inverse);

        // cv::threshold(new_cells, inverse, unknown-1, 255, cv::THRESH_BINARY_INV);
        cv::bitwise_not(inverse, inverse);
        

        cv::imshow( "inverse", inverse );
        cv::waitKey(0);



        int connected_count = connectedComponents(inverse, labelImage, 8);
        std::vector<cv::Vec3b> colors(connected_count);
        colors[0] = cv::Vec3b(0, 0, 0);//background
        for(int label = 1; label < connected_count; ++label){
            colors[label] = cv::Vec3b( 255, 255 - 50*label, 50*label + 50 );
        }
        cv::Mat dst(new_cells.size(), CV_8UC3);
        for(int r = 0; r < dst.rows; ++r){
            for(int c = 0; c < dst.cols; ++c){
                int label = labelImage.at<int>(r, c);
                cv::Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
                pixel = colors[label];
            }
        }
        cv::imshow( "Connected Components", dst );
        cv::waitKey(0);

        double connected_mod = -500;

        score += connected_mod * connected_count;
#ifdef COST_VEC_PRINT
        std::cout << " Connected Components Penalty: " << connected_mod * connected_count;
#endif







#ifdef COST_VEC_PRINT
        std::cout  << " Final Score: " << score <<  std::endl;  
#endif      
        if(score > best_score){
            best_score = score;
            best_point = f;
        }
        
    }


    // //Finf min arg
    // std::pair<cv::Point2i,double>result = *std::max_element(cost_vec.cbegin(), cost_vec.cend(), [](const std::pair<cv::Point2i,double> &lhs, const std::pair<cv::Point2i,double> &rhs) {
    //     return lhs.second < rhs.second;    
    // });


    std::cout <<  "BEST! " << "Point:" << best_point.x <<"," << best_point.y << " Score: " << best_score << std::endl;
    cv::waitKey(10);


    return this->point2Pair(best_point);


}

void Agent::moveToPosition(std::pair<int,int> pos){
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
}



void Agent::measureSignalStrength(Field f) {
    std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords);
    std::vector<Ring> curr_est;
    for(auto &m: measurements){
        std::cout << m.first << ": " << m.second << std::endl;
        
        
        Ring r = Ring(this->field_x_width, this->field_y_length, this->coords.first, this->coords.second,m.second, 1, m.first ); 
        r.drawRing();
        cv::Mat img = r.getCanvas();
        cv::imshow("New Measurement", img);
        cv::waitKey(0);
        
        if(this->signal_estimations[r.getName()].size() == 0){
            this->signal_estimations[r.getName()] = curr_est;
        }
        this->signal_estimations[r.getName()].push_back(r);


    }
    this->measurements.push_back(measurements);

    for(auto &key_val: this->signal_estimations){
        cv::Mat img = Ring::intersectRings(key_val.second);
        cv::imshow(key_val.first, img);
        cv::waitKey(10);
    }

    
}





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