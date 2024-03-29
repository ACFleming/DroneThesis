#include "Agent.hpp"






int Agent::step_counter = 0;

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

Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length,  double scan_radius, double measurement_std_dev, double speed, std::map<std::string, Grid> *certainty_grids) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->name = name;
    
    this->field_x_rows = field_width;
    this->field_y_cols = field_length;
    this->scan_radius = scan_radius;
    this->measurement_std_dev = measurement_std_dev*1.5; //Factor of safety
    this->speed = speed;
    
    this->certainty_grids = certainty_grids;
    
    this->certainty_grids->insert(std::pair<std::string, Grid>(BASE,Grid(BASE, this->field_x_rows, this->field_y_cols)));
    
    this->certainty_grids->insert(std::pair<std::string, Grid>(MAP, Grid(MAP,  this->field_x_rows, this->field_y_cols)));
    
    this->output = &std::cout;
    *this->output << name << " fininshed loading" << std::endl;

    

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
    cv::Mat mask = cv::Mat::zeros(this->field_y_cols,this->field_x_rows, CV_8UC1);
    cv::Point centre(x,y);
    cv::circle(mask, centre, this->scan_radius, cv::Scalar(value,value,value), -1);
    return mask;
}



void Agent::recordCommand(std::pair<int,int> p ){
    if(this->command_history.count(p) == 0){
        this->command_history.insert(std::make_pair(p, 0));
    }
    this->command_history.at(p) = this->command_history.at(p)+1;
}

int Agent::countCommand(std::pair<int,int> p){
    if(this->command_history.count(p) == 0){
        return 0;
    }
    return this->command_history.at(p);
}






void Agent::updateCertainty(Field f){

    

    //step 0 clear old measurements



    for(auto &kv_name_grid: *(this->certainty_grids)){
        kv_name_grid.second.prepareForUpdate(this->coords, this->scan_radius);
    }
    

    //step 1 get any signal measurements

    std::map<std::string, std::vector<double>> all_measurements;
    for(int repeat_ping = 0; repeat_ping < 10; repeat_ping++){
        
        std::vector<std::pair<std::string, double>> raw_measurement = f.getMeasurements(this->coords);
        for(auto &m: raw_measurement){
            if(all_measurements.count(m.first) == 0){
                // std::pair<
                all_measurements.insert(std::pair<std::string, std::vector<double>>(m.first,{}));
            }
            all_measurements.at(m.first).push_back(m.second);
        }
    }



    std::map<std::string, double> avg_measurements;

    for(auto &all: all_measurements){
        avg_measurements.insert(std::pair<std::string, double>(all.first, std::accumulate(all.second.begin(), all.second.end(), 0.0)/all.second.size()));
    }


    for(auto &m: avg_measurements){
        // std::cout << "" <<  m.first << " AVG:" << m.second << std::endl;
        if(this->certainty_grids->count(m.first) == 0){ //if no existing grid region
            cv::Mat base = this->certainty_grids->at(BASE).getLikelihood().clone();
            // cv::threshold(base, base, searching-1, 255, cv::THRESH_BINARY);
#ifdef SHOW_IMG
        cv::imshow("init signal likelihood", base);
        cv::waitKey(WAITKEY_DELAY);
#endif
            this->certainty_grids->insert(std::pair<std::string, Grid> (m.first, Grid(m.first, base)));
            this->certainty_grids->at(m.first).prepareForUpdate(this->coords, this->scan_radius);
        }
        *this->output << "Signal name: " << "," << m.first << "," << " Signal value: " << "," << m.second << std::endl;
        this->certainty_grids->at(m.first).receiveMeasurement(m.second, this->measurement_std_dev);

    }

    for(auto &kv_name_grid: *(this->certainty_grids)){
        kv_name_grid.second.updateCertainty();
#ifdef SHOW_IMG
        cv::imshow(kv_name_grid.first,kv_name_grid.second.getLikelihood() );
        cv::waitKey(WAITKEY_DELAY);
#endif        
    }

    

}



cv::Mat Agent::validateGrid(cv::Mat in){
    cv::Mat valid_zone = this->certainty_grids->at(MAP).getLikelihood().clone();
    cv::threshold(valid_zone, valid_zone, 0, 255, cv::THRESH_BINARY);
    cv::bitwise_and(valid_zone, in, valid_zone);
    return valid_zone;
}



void Agent::costFunction(std::vector<cv::Point2i> points, std::unordered_set<cv::Point,point_hash> signal_frontiers, std::unordered_set<cv::Point,point_hash> hole_centres,  cv::Point2i &best_point, double &best_score){
#ifdef COST_VEC_PRINT
        *this->output << "At: " << ","  << this->coords.first << "," << this->coords.second << "," << std::endl;
#endif
    for(auto &f: points ){
        // tic();F
#ifdef COST_VEC_PRINT
        *this->output << "Point: " << ","  << f.x << "," << f.y << ",";
#endif
        
        cv::Mat seen = cv::Mat::zeros(this->field_y_cols,this->field_x_rows,CV_8UC3); 
        // cv::threshold(this->certainty_grids->at(BASE).getLikelihood(), seen,searching-1, 255, cv::THRESH_BINARY_INV);
        cv::inRange(this->certainty_grids->at(BASE).getLikelihood(),cleared, cleared, seen);

#ifdef SHOW_IMG
    cv::imshow("seen", seen);
    cv::waitKey(WAITKEY_DELAY);
#endif

        cv::Mat remaining = cv::Mat::zeros(this->field_y_cols,this->field_x_rows,CV_8UC3); 
        // cv::threshold(this->certainty_grids->at(BASE).getLikelihood(), seen,searching-1, 255, cv::THRESH_BINARY_INV);
        cv::inRange(this->certainty_grids->at(BASE).getLikelihood(),searching, searching, remaining);

#ifdef SHOW_IMG
    cv::imshow("remaining", remaining);
    cv::waitKey(WAITKEY_DELAY);
#endif

        cv::Mat new_cells = cv::Mat::zeros(this->field_y_cols,this->field_x_rows,CV_8UC3); 
        cv::Mat new_remaining = cv::Mat::zeros(this->field_y_cols,this->field_x_rows,CV_8UC3); 
        std::vector<std::vector<cv::Point2i>> new_frontiers;

        double score = 0;

        // if(this->numberOfVisits(this->point2Pair(f)) > 1){
        //     double repeat_point_mod = -100000;
            
        //     score += repeat_point_mod;
        //     *this->output << " Repeat point!";
        // }
        double repeat_cmd_mod = -1;

        double repeat_cmd_score = pow(100, this->countCommand(this->point2Pair(f)));

        score += repeat_cmd_mod * repeat_cmd_score;

#ifdef COST_VEC_PRINT
        *this->output << "Repeat score:" << "," << repeat_cmd_score << "," << " Repeat contribution: " << "," << repeat_cmd_mod * repeat_cmd_score << ",";
#endif

 



        if(signal_frontiers.count(f) > 0 ){ //if its a signal frontier point;
            double signal_mod = 500;
            // signal_mod = 0;
            score += signal_mod;
        }

        // if(hole_centres.count(f) > 0 ){ //if its a signal frontier point;
        //     double hole_mod = 1000;
        //     // hole_mod = 0;
        //     // score += hole_mod;
        // }


        // double dist = this->dist(this->coords, this->point2Pair(f));
        double horizontal_dist = abs(this->coords.first - f.x);
        double vertical_dist = abs(this->coords.second - f.y); 
        double horiz_mod = 1;
        double vert_mod = 1.3;
        double distance = hypot(horiz_mod*horizontal_dist, vert_mod*vertical_dist);


        double dist_mod = -5;

#ifndef DIST
        dist_mod = 0;
#endif
        
        score += dist_mod  * distance;
        // score += dist_mod*exp(dist/this->scan_radius);

#ifdef COST_VEC_PRINT
        *this->output << "Dist:" << "," << distance << "," << " Dist contribution: " << "," << dist_mod * distance << ",";
#endif

        cv::bitwise_or(seen, this->rangeMask(f.x, f.y, searching), new_cells);  
        new_cells = this->validateGrid(new_cells);
#ifdef SHOW_IMG
        cv::imshow("new_cells", new_cells);
        cv::waitKey(WAITKEY_DELAY);
#endif






    






        double old_scanned_count = cv::countNonZero(seen);
        double new_scanned_count = cv::countNonZero(new_cells);

        double new_scanned_ratio = (new_scanned_count-old_scanned_count)/old_scanned_count;
        

        //hyperbolic scoring for seen

        
        double scanned_mod = 0.5*100;

#ifndef SEEN
        scanned_mod = 0;
#endif        
        
        score += scanned_mod * new_scanned_ratio;

        
#ifdef COST_VEC_PRINT
        *this->output << " New scanned ratio: "<<  "," << new_scanned_ratio << "," << " New Scanned Cells Contribution: " << "," <<  scanned_mod  *new_scanned_ratio << ",";
#endif



        int frontier_chains = this->frontiers.size();

        
        new_frontiers = Grid::getImageFrontiers(new_cells);


//     //global dist

//         double global_dist = 0;
//         for(auto &frontier_vec : new_frontiers){
//             for(auto &nf: frontier_vec){
//                 global_dist += this->dist(point2Pair(nf), point2Pair(f));
//             }
//         }

//         double global_dist_mod = -0.001;
//         global_dist_mod = 0;
//         score += global_dist_mod * global_dist;
// #ifdef COST_VEC_PRINT
//         *this->output << " Global Dist "<<  "," <<global_dist << "," << " Global Dist Contribution: " << "," <<  global_dist_mod * global_dist << ",";
// #endif




        for(auto &nf : new_frontiers){
            
            double signed_dist = cv::pointPolygonTest(nf, this->pair2Point(this->coords), true);
            if(signed_dist < 0){
                continue;
            }

            double ctr_area = cv::contourArea(nf);
            double ctr_perim = cv::arcLength(nf, true);
            std::vector<cv::Point2i> hull_points;

            cv::convexHull(nf, hull_points);
            double hull_area = cv::contourArea(hull_points);
            double hull_perim = cv::arcLength(hull_points, true);
            if(hull_area == 0){
                // *this->output << "HUH?" << std::endl;
                hull_area = 1;
            }

            cv::Mat hull_img = cv::Mat::zeros(this->field_y_cols, this->field_x_rows, CV_8UC3);
            cv::drawContours(hull_img, new_frontiers, -1, cv::Scalar(0,0,255));
            
            std::vector<std::vector<cv::Point2i>> h;
            h.push_back(hull_points);
            cv::drawContours(hull_img, h, -1, cv::Scalar(255,0,0));

#ifdef SHOW_IMG
            cv::imshow("hull_img", hull_img);
            cv::waitKey(WAITKEY_DELAY);
#endif
            double area_rt = (ctr_area)/hull_area;

            double area_rt_mod = 3*10;
#ifndef HULL_AREA
            area_rt_mod = 0;
#endif
            score += area_rt_mod * area_rt;
#ifdef COST_VEC_PRINT
            *this->output << "Area ratio: " << "," << area_rt << "," << " Area ratio Contribution: " << "," << area_rt_mod * area_rt << ",";
#endif

            double perim_diff = abs(ctr_perim);

            double perim_mod = -0.3;

#ifndef HULL_PERIM
            perim_mod = 0;
#endif
            score += perim_mod * perim_diff;
#ifdef COST_VEC_PRINT
            *this->output << "Perim diff: " << "," << perim_diff << "," << " Perim diff Contribution: " << "," << perim_mod * perim_diff << ",";
#endif

        }





// //Inverse concavity check


        cv::Mat inverse(new_cells.size(), CV_8UC1);

        cv::threshold(new_cells, inverse, searching-1, 255, cv::THRESH_BINARY);
        cv::bitwise_not(inverse, inverse);
        cv::bitwise_and(inverse, remaining, inverse);


        // cv::inRange(this->certainty_grids->at(BASE).getLikelihood(),searching, searching, inverse);
#ifdef SHOW_IMG
        cv::imshow("inverse", inverse );
        cv::waitKey(WAITKEY_DELAY);
#endif




        std::vector<std::vector<cv::Point>> inverse_frontiers = Grid::getImageFrontiers(inverse);

            // double frontier_chain_penalty = pow(100, (int)(inverse_frontiers.size()) -1);
            // double f_chain_mod = -1;
            // f_chain_mod = 0;
            // score += f_chain_mod * frontier_chain_penalty;
            
// #ifdef COST_VEC_PRINT   
//             *this->output << "Frontier contour count: " << "," << inverse_frontiers.size()-1 << "," << " Hole count Contribution " << ","<< f_chain_mod * frontier_chain_penalty << ",";
// #endif

        if(inverse_frontiers.size() > 0){
            double inv_ctr_perim = 0;
            double inv_hull_perim = 0;
            double inv_ctr_area = 0;
            double inv_hull_area = 0;
            // if(inverse_frontiers.size() > 1){
            //     NO_OP;
            // }


            int frontier_chain_diff = frontier_chains - inverse_frontiers.size();
            double chain_diff_mod = 300;
#ifndef CHAIN
            chain_diff_mod = 0;
#endif
            score += chain_diff_mod * frontier_chain_diff;


#ifdef COST_VEC_PRINT   
            *this->output << "Frontier diff: " << "," << frontier_chain_diff << "," << " Frontier diff Contribution " << ","<< chain_diff_mod * frontier_chain_diff << ",";
#endif

            


            for(auto &iif : inverse_frontiers){
                inv_ctr_area += cv::contourArea(iif);
                inv_ctr_perim += cv::arcLength(iif, true);
               std::vector<cv::Point2i> inv_hull_points;

                cv::convexHull(iif, inv_hull_points);
                inv_hull_area += cv::contourArea(inv_hull_points);
                inv_hull_perim += cv::arcLength(inv_hull_points, true);
                // std::cout << "arc closed: " << cv::arcLength(inv_hull_points, true) << " arc open: " << cv::arcLength(inv_hull_points, false) << std::endl;
                if(inv_hull_area == 0){
                    // *this->output << "HUH?" << std::endl;
                    inv_hull_area = 1;  
                }

// #ifdef COST_VEC_PRINT   
//                 *this->output << "Small frontier Contribution: " << "," << small_frontier_mod << ",";
// #endif

                cv::Mat inv_hull_img = cv::Mat::zeros(this->field_y_cols, this->field_x_rows, CV_8UC3);
                cv::drawContours(inv_hull_img, inverse_frontiers, -1, cv::Scalar(0,0,255));
                
                std::vector<std::vector<cv::Point2i>> inv_h = std::vector<std::vector<cv::Point2i>>();
                inv_h.push_back(inv_hull_points);
                cv::drawContours(inv_hull_img, inv_h, -1, cv::Scalar(255,0,0));

#ifdef SHOW_IMG
                cv::imshow("inv_hull_img", inv_hull_img);
                cv::waitKey(WAITKEY_DELAY);
#endif
            }


            double inv_area_ratio = inv_ctr_area/inv_hull_area;

            double inv_area_rt_mod = 7*100;

#ifndef INV_HULL_AREA

            inv_area_rt_mod = 0;
#endif

            score += inv_area_rt_mod * inv_area_ratio;

            // if(inv_hull_perim == 0) inv_hull_perim = 1;
            // double inv_perim_ratio = inv_ctr_perim/inv_hull_perim;

            // double inv_perim_rt_mod = -3*pow(100, inverse_frontiers.size());
            // inv_perim_rt_mod = 0;

            
            // score += inv_perim_rt_mod * inv_perim_ratio;
#ifdef COST_VEC_PRINT   
            *this->output << "Inverse Area ratio: " << "," << inv_area_ratio << "," << " Inv Area Ratio Contribution: " << "," << inv_area_rt_mod * inv_area_ratio << ",";
#endif
        }

            










#ifdef COST_VEC_PRINT
        *this->output  << " Final Score: " << "," << score <<  std::endl;  
#endif      
        if(score > best_score){
            best_score = score;
            best_point = f;
        }


        // cv::waitKey(0);
        // toc();
        
    }
    

}


std::pair<int,int> Agent::determineAction(){
 

    std::vector<cv::Point2i> missed_spots;
    cv::Mat missed_map = this->certainty_grids->at(BASE).getLikelihood().clone();
    cv::threshold(this->certainty_grids->at(BASE).getLikelihood(), missed_map, searching-1, 255, cv::THRESH_BINARY);
    cv::findNonZero(missed_map,missed_spots);
    


    if(missed_spots.size() == 0){ //fully explored
        // *this->output <<"DONE!" << std::endl;
        return std::make_pair(-1,-1);
    }


    std::pair<int,int> next_position = this->coords;

    




    double best_score = LONG_MIN;
    cv::Point2i best_point = cv::Point2i(-1,-1);

// #ifdef SHOW_IMG
//     cv::imshow("BASE", this->certainty_grids->at(BASE).getLikelihood());
//     cv::waitKey(WAITKEY_DELAY);
// #endif


        cv::Mat seen = cv::Mat::zeros(this->field_y_cols,this->field_x_rows,CV_8UC3); 
        // cv::threshold(this->certainty_grids->at(BASE).getLikelihood(), seen,searching-1, 255, cv::THRESH_BINARY_INV);
        cv::inRange(this->certainty_grids->at(BASE).getLikelihood(),cleared, cleared, seen);

#ifdef SHOW_IMG
    cv::imshow("seen", seen);
    cv::waitKey(WAITKEY_DELAY);
#endif

        cv::Mat remaining = cv::Mat::zeros(this->field_y_cols,this->field_x_rows,CV_8UC3); 
        // cv::threshold(this->certainty_grids->at(BASE).getLikelihood(), seen,searching-1, 255, cv::THRESH_BINARY_INV);
        cv::inRange(this->certainty_grids->at(BASE).getLikelihood(),searching, searching, remaining);

#ifdef SHOW_IMG
    cv::imshow("remaining", remaining);
    cv::waitKey(WAITKEY_DELAY);
#endif






    this->frontiers = Grid::getImageFrontiers(remaining);
    int frontier_chains = this->frontiers.size();
    if(frontier_chains == 0){
        //pixels remaining but no frontiers???
        exit(7);
    }






    //need to define hash function for cv::Point set



    

    std::unordered_set<cv::Point,point_hash> signal_frontiers;
    std::unordered_set<cv::Point,point_hash> hole_centres;

    std::vector<cv::Point> priority_frontiers;
    std::vector<cv::Point> reserve_frontiers;




    cv::Mat priority_img = cv::Mat::zeros(this->field_y_cols,this->field_x_rows,CV_8UC3);    
    
    //filter frontier points based on distance
    for(auto &f_vec: this->frontiers){
        for(auto &f: f_vec){
            double dist = this->dist(this->coords, this->point2Pair(f));
            int edge_root_func = (f.x*(field_x_rows-1-f.x)) + (f.y*(field_y_cols-1-f.y));
            if( edge_root_func == 0 || cv::pointPolygonTest(this->certainty_grids->at(MAP).getMapEdges(), f, false)  < 0){
                continue;
            }   
            if(dist < 2 || dist > 2*this->speed){
                cv::circle(priority_img, f,2,cv::Scalar(255,0,255));
                reserve_frontiers.push_back(f);
            }else{
                priority_frontiers.push_back(f);
                cv::circle(priority_img, f,2,cv::Scalar(0,255,255));
            }
        }
    }

    //add signal bounds to the priority frontier list
    for(auto &kv_name_grid: *(this->certainty_grids)){
        if(kv_name_grid.second.isFound() == false && kv_name_grid.first != BASE && kv_name_grid.first != MAP){//if not found
            for(auto &p: kv_name_grid.second.getSignalBounds().getAll()){
                double dist_to_edge = cv::pointPolygonTest(this->certainty_grids->at(MAP).getMapEdges(), p, false);
                if(cv::pointPolygonTest(this->certainty_grids->at(MAP).getMapEdges(), p, false)  >= 0){
                    signal_frontiers.insert(p);
                    priority_frontiers.push_back(p);
                    cv::circle(priority_img, p,2,cv::Scalar(255,255,0));
                }

            }
        }
    }


    *this->output << "Total frontiers: " << "," << this->frontiers[0].size() << "," << " Prioritised frontiers: " << "," << priority_frontiers.size() << std::endl;
    
    
// #ifdef SHOW_IMG
    cv::imshow("Filtered Frontiers", priority_img);
    cv::waitKey(WAITKEY_DELAY);
// #endif
    

    this->costFunction(priority_frontiers, signal_frontiers,hole_centres,  best_point,best_score);

//COST FUNCTION


    
    if(best_score < -10000){ 
        // *this->output << "SHOULD BE DONE!" << std::endl;
        std::vector<cv::Point2i> missed_spots;
        cv::Mat remaining = this->certainty_grids->at(BASE).getLikelihood().clone();
        cv::threshold(this->certainty_grids->at(BASE).getLikelihood(), remaining, searching-1, 255, cv::THRESH_BINARY);
        cv::findNonZero(remaining,missed_spots);
// #ifdef SHOW_IMG
//         cv::imshow("done yet?",remaining);
//         cv::waitKey(0);
// #endif

        if(missed_spots.size() == 0){ //fully explored
            // *this->output <<"DONE!" << std::endl;
            return std::make_pair(-1,-1);
        }
        *this->output << "Checking reserve frontiers" << std::endl;
        this->costFunction(reserve_frontiers, signal_frontiers, hole_centres,  best_point,best_score);




        if(best_score < -100000){ 
            *this->output << "Checking for remaining cells" << std::endl;
            double missed_p_dist = this->field_x_rows*this->field_x_rows + this->field_y_cols*this->field_y_cols;
            for(auto &p: missed_spots){
                if(missed_p_dist > this->dist(this->coords, this->point2Pair(p))){
                    best_point = p;
                    best_score = 0;
                    missed_p_dist = this->dist(this->coords, this->point2Pair(p));
                }
            }
        }
        
        


        

    }




    *this->output <<  "BEST Point:" << "," << best_point.x << "," << best_point.y << "," << " Score: " << "," << best_score << std::endl;
    cv::Mat best = this->certainty_grids->at(BASE).getLikelihood().clone();
    cv::circle(best, best_point, 3, cv::Scalar(255));
#ifdef SHOW_IMG
    cv::imshow("best", best);
    cv::waitKey(WAITKEY_DELAY);
#endif
    this->recordCommand(this->point2Pair(best_point));
    *this->output << "COUNT COMMAND: " << this->countCommand(this->point2Pair(best_point)) << std::endl;
    return this->point2Pair(best_point);


}

std::pair<int,int> Agent::moveToPosition(std::pair<int,int> pos){
    // this->certainty_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
    // std::cout << pos.first << '.' << pos.second << std::endl;
    pos.first = this->clipRange(0, this->field_x_rows, pos.first);
    pos.second = this->clipRange(0, this->field_y_cols, pos.second);

    std::pair<int,int> interpolated_pos = pos;

    double factor = this->speed/this->dist(pos, this->coords);
    

    // std::cout << factor << std::endl;
    
    if(factor < 1){
        interpolated_pos.first = (pos.first-this->coords.first)*factor + this->coords.first;
        interpolated_pos.second = (pos.second-this->coords.second)*factor + this->coords.second;
    }else{
        interpolated_pos.first = pos.first;
        interpolated_pos.second = pos.second;
    }

    

    // *this->output << "Interpolation factor: " << factor << " Interpolated point: " << interpolated_pos.first << "," << interpolated_pos.second << std::endl;


    interpolated_pos.first = this->clipRange(0, this->field_x_rows, interpolated_pos.first);
    interpolated_pos.second = this->clipRange(0, this->field_y_cols, interpolated_pos.second);



    this->coords = interpolated_pos;
    // this->certainty_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
    
    Agent::step_counter++;

    return interpolated_pos;
}







std::pair<int,int> Agent::getCoords(){
    return this->coords;
}


cv::Mat Agent::getMap(){
    return this->certainty_grids->at(BASE).getLikelihood();
}

cv::Mat Agent::getSignalLocations() {
    cv::Mat confirmed = cv::Mat::zeros(this->field_y_cols, this->field_x_rows, CV_8UC3);
    cv::Mat temp = cv::Mat::zeros(this->field_y_cols, this->field_x_rows, CV_8UC3);
    for(auto &kv_name_grid: *(this->certainty_grids)){
        if(kv_name_grid.second.isFound() && kv_name_grid.first != BASE){
            temp = kv_name_grid.second.getLikelihood().clone();
            cv::cvtColor(temp, temp, cv::COLOR_GRAY2BGR);
            // cv::imshow(kv_name_grid.first+" coloured", temp);
            // cv::waitKey(0);

            // cv::cvtColor(temp, temp, cv::COLOR_BGR2HSV);
            // srand((unsigned int)(time(0)));
            std::hash<std::string> colour_name_hasher;
            uint hash_colour = colour_name_hasher(kv_name_grid.first);
            int colour1 = hash_colour%200+55;
            int colour2 = (hash_colour/1000)%200+55;
            int colour3 = (hash_colour/1000000)%200+55;

            temp.setTo(cv::Scalar(colour1, colour2, colour3), temp);
            cv::cvtColor(temp, temp, cv::COLOR_HSV2BGR);
            // cv::imshow(kv_name_grid.first+" coloured", temp);
            // cv::waitKey(0);

            cv::bitwise_or(confirmed, temp, confirmed);
            
        }

        
    }
    return confirmed;
}


bool Agent::verifySignalLocations(std::string name, std::pair<int,int> true_location){



    cv::Mat mask = this->certainty_grids->at(name).getLikelihood().clone(); 
    



    //now the black region is where the signal should be
    cv::bitwise_not(mask, mask);
#ifdef SHOW_IMG
    cv::imshow("Mask for", mask);
    cv::waitKey(0);
#endif


    
    cv::Mat tmp = cv::Mat::zeros(mask.size(), CV_8UC1);
    // cv::circle(tmp, this->pair2Point(true_location), 1, cv::Scalar(255));
    tmp.at<uint8_t>(this->pair2Point(true_location))= 255;

#ifdef SHOW_IMG
    cv::imshow("True location", tmp);
    cv::waitKey(0);
#endif

    cv::bitwise_and(tmp, mask, tmp);

    
#ifdef SHOW_IMG
    cv::imshow("Missed overlap", tmp);
    cv::waitKey(0);
#endif

    int remaining = cv::countNonZero(tmp);

    if(remaining == 0){
        *this->output << "Signal name: " << "," << name << "," <<  "x:"  << "," <<  true_location.first << "," << "y:" << "," << true_location.second  << "," << "Inside likelihood area? : " << "," << "True" << std::endl;
        std::cout << "Signal name: " << "," << name << "," <<  "x:"  << "," <<  true_location.first << "," << "y:" << "," << true_location.second  << "," << "Inside likelihood area? : " << "," << "True" << std::endl;
        return true;
    }else{
        *this->output << "Signal name: " << "," << name << "," <<  "x:"  << "," <<  true_location.first << "," << "y:" << "," << true_location.second  << "," << "Inside likelihood area? : " << "," << "False" << std::endl;
        std::cout << "Signal name: " << "," << name << "," <<  "x:"  << "," <<  true_location.first << "," << "y:" << "," << true_location.second  << "," << "Inside likelihood area? : " << "," << "False" << std::endl;
        exit(1);
        return false;
    }
    


//     //as this is after completion, this is the 3 std dev likelihood


//     // cv::threshold()
}



void Agent::logAgent() {
    
    *this->output << "Steps taken: " << "," << Agent::step_counter << std::endl;
    for(auto &kv_name_grid: *(this->certainty_grids)){
        if(kv_name_grid.first == BASE || kv_name_grid.first == MAP ) continue;
        *this->output << kv_name_grid.first << ","  << kv_name_grid.second.getSignalBounds().getCentre().x << " , " << kv_name_grid.second.getSignalBounds().getCentre().y << std::endl;
        *this->output << "Likelihood area: " << "," << kv_name_grid.second.getSignalBounds().getArea() << std::endl;
        
    }
}





