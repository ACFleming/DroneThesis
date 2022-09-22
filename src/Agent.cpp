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

Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius, int speed, std::map<std::string, Grid> *certainty_grids) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->name = name;
    this->field_x_width = field_width;
    this->field_y_length = field_length;
    this->scan_radius = scan_radius;
    this->speed = speed;
    this->certainty_grids = certainty_grids;
    this->certainty_grids->insert(std::pair<std::string, Grid>(BASE,Grid(this->field_x_width, this->field_y_length, unknown)));
    this->certainty_grids->insert(std::pair<std::string, Grid>(MAP,Grid(this->field_x_width, this->field_y_length, unknown)));
    this->output = &std::cout;


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

    //Note! Need to update certainty grid of base after move, to update position

    //step 0 clear old measurements



    for(auto &kv_name_grid: *(this->certainty_grids)){
        kv_name_grid.second.prepareForUpdate(this->coords, this->scan_radius);
    }
    

    //step 1 get any signal measurements


    std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords); //to change for BT ping


    for(auto &m: measurements){

        if(this->certainty_grids->count(m.first) == 0){ //if no existing grid region
            this->certainty_grids->insert(std::pair<std::string, Grid> (m.first, Grid(m.first, this->certainty_grids->at(BASE).getLikelihood().clone())));
            this->certainty_grids->at(m.first).prepareForUpdate(this->coords, this->scan_radius);
        }
        *this->output << "Signal name: " << "," << m.first << "," << " Signal value: " << "," << m.second << std::endl;
        this->certainty_grids->at(m.first).receiveMeasurement(m.second);

    }

    for(auto &kv_name_grid: *(this->certainty_grids)){
        kv_name_grid.second.updateCertainty();
    }

    

}

void Agent::updateMap(){
    this->certainty_grids->at(MAP).prepareForUpdate(this->coords, this->scan_radius);
    this->certainty_grids->at(MAP).updateCertainty();
    // this->certainty_grids->at(MAP).getLikelihood().at<uint8_t>(this->pair2Point(this->coords)) = occupied;
}


void Agent::costFunction(cv::Mat seen, std::vector<cv::Point2i> points, std::unordered_set<cv::Point,point_hash> signal_frontiers, cv::Point2i &best_point, double &best_score){
    for(auto &f: points ){
#ifdef COST_VEC_PRINT
        *this->output << "Point: " << ","  << f.x << "," << f.y << ",";
#endif
        
        cv::Mat new_cells = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC3); 
        std::vector<std::vector<cv::Point2i>> new_frontiers;

        double score = 0;

        // if(this->numberOfVisits(this->point2Pair(f)) > 1){
        //     double repeat_point_mod = -100000;
            
        //     score += repeat_point_mod;
        //     *this->output << " Repeat point!";
        // }
        double repeat_cmd_mod = -1;

        double repeat_cmd_score = pow(10, this->countCommand(this->point2Pair(f)) - 1);

        score += repeat_cmd_mod * repeat_cmd_score; 



        if(signal_frontiers.count(f) > 0 ){ //if its a signal frontier point;
            score += 50;
        }



        double dist = this->dist(this->coords, this->point2Pair(f));


        double dist_mod = -0.05;
        
        score += dist_mod * (dist-(this->speed)/2) * (dist-1.5*this->speed);
        // score += dist_mod*exp(dist/this->scan_radius);

#ifdef COST_VEC_PRINT
        *this->output << "Dist:" << "," << dist << "," << " Dist contribution: " << "," << dist_mod * dist << ",";
#endif

        cv::bitwise_or(seen, this->rangeMask(f.x, f.y, searching), new_cells);  
#ifdef SHOW_IMG
        cv::imshow("new_cells", new_cells);
        cv::waitKey(WAITKEY_DELAY);
#endif


        double old_scanned_count = cv::countNonZero(seen);
        double new_scanned_count = cv::countNonZero(new_cells);

        double new_scanned_ratio = (new_scanned_count-old_scanned_count)/old_scanned_count;
        
        double scanned_mod = 1.5*100;
        // scanned_mod = 0;



        score += scanned_mod * new_scanned_ratio;
#ifdef COST_VEC_PRINT
        *this->output << " New scanned ratio: "<<  "," << new_scanned_ratio << "," << " New Scanned Cells Contribution: " << "," <<  scanned_mod  *new_scanned_ratio << ",";
#endif




        new_frontiers = Grid::getImageFrontiers(new_cells);

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

            cv::Mat hull_img = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC3);
            cv::drawContours(hull_img, new_frontiers, -1, cv::Scalar(0,0,255));
            
            std::vector<std::vector<cv::Point2i>> h;
            h.push_back(hull_points);
            cv::drawContours(hull_img, h, -1, cv::Scalar(255,0,0));

#ifdef SHOW_IMG
            cv::imshow("hull_img", hull_img);
            cv::waitKey(WAITKEY_DELAY);
#endif
            double area_ratio = ctr_area/hull_area;

            double area_rt_mod = 3*100;

            // area_rt_mod = 0;

            score += area_rt_mod * area_ratio;
#ifdef COST_VEC_PRINT
            *this->output << "Area ratio: " << "," << area_ratio << "," << " Area Ratio Contribution: " << "," << area_rt_mod * area_ratio << ",";
#endif

        }





//Inverse concavity check


        cv::Mat inverse(new_cells.size(), CV_8UC1);
        cv::threshold(new_cells, inverse, searching-1, 255, cv::THRESH_BINARY);
        cv::bitwise_not(inverse, inverse);
#ifdef SHOW_IMG
        cv::imshow("inverse", inverse );
        cv::waitKey(WAITKEY_DELAY);
#endif




        std::vector<std::vector<cv::Point>> inverse_frontiers = Grid::getImageFrontiers(inverse);


        if(inverse_frontiers.size() > 0){
            double inv_ctr_perim = 0;
            double inv_hull_perim = 0;
            double inv_ctr_area = 0;
            double inv_hull_area = 0;
            // if(inverse_frontiers.size() > 1){
            //     NO_OP;
            // }
            for(auto &iif : inverse_frontiers){
                inv_ctr_area += cv::contourArea(iif);
                inv_ctr_perim += cv::arcLength(iif, true);
                std::vector<cv::Point2i> inv_hull_points;

                cv::convexHull(iif, inv_hull_points);
                inv_hull_area += cv::contourArea(inv_hull_points);
                inv_hull_perim += cv::arcLength(inv_hull_points, true);
                if(inv_hull_area == 0){
                    // *this->output << "HUH?" << std::endl;
                    inv_hull_area = 1;
                }

                cv::Mat inv_hull_img = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC3);
                cv::drawContours(inv_hull_img, inverse_frontiers, -1, cv::Scalar(0,0,255));
                
                std::vector<std::vector<cv::Point2i>> inv_h = std::vector<std::vector<cv::Point2i>>();
                inv_h.push_back(inv_hull_points);
                cv::drawContours(inv_hull_img, inv_h, -1, cv::Scalar(255,0,0));

#ifdef SHOW_IMG
                cv::imshow("inv_hull_img", inv_hull_img);
                cv::waitKey(WAITKEY_DELAY);
#endif
            }




            if(inv_hull_perim == 0) inv_hull_perim = 1;
            double inv_perim_ratio = inv_ctr_perim/inv_hull_perim;

            double inv_perim_rt_mod = -3*100;
            // inv_perim_rt_mod = 0;

            
            score += inv_perim_rt_mod * inv_perim_ratio;
#ifdef COST_VEC_PRINT   
            *this->output << "Inverse perim ratio: " << "," << inv_perim_ratio << "," << " Perim Ratio Contribution: " << "," << inv_perim_rt_mod * inv_perim_ratio << ",";
#endif
        }

            
            double hole_value = pow(100, (int)(inverse_frontiers.size()) -1);
            double hole_mod = -1;
            // hole_mod = 0;
            score += hole_mod * hole_value;
            
#ifdef COST_VEC_PRINT   
            *this->output << "Hole count: " << "," << inverse_frontiers.size()-1 << "," << " Hole count Contribution " << ","<< hole_mod * hole_value << ",";
#endif









#ifdef COST_VEC_PRINT
        *this->output  << " Final Score: " << "," << score <<  std::endl;  
#endif      
        if(score > best_score){
            best_score = score;
            best_point = f;
        }


        // cv::waitKey(0);
        
    }

}


std::pair<int,int> Agent::determineAction(){
 


    std::pair<int,int> next_position = this->coords;

    




    double best_score = LONG_MIN;
    cv::Point2i best_point = cv::Point2i(-1,-1);



    cv::Mat seen = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC3); 
    cv::threshold(this->certainty_grids->at(MAP).getLikelihood(), seen,unknown-1, unknown, cv::THRESH_BINARY_INV);

    this->frontiers = Grid::getImageFrontiers(seen);


#ifdef SHOW_IMG
    cv::imshow("seen", seen);
    cv::waitKey(WAITKEY_DELAY);
#endif



    //need to define hash function for cv::Point set



    

    std::unordered_set<cv::Point,point_hash> signal_frontiers;

    std::vector<cv::Point> priority_frontiers;
    std::vector<cv::Point> reserve_frontiers;




    cv::Mat priority_img = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC3);    
    
    //filter frontier points based on distance
    for(auto &f_vec: this->frontiers){
        for(auto &f: f_vec){
            double dist = this->dist(this->coords, this->point2Pair(f));
            int edge_root_func = (f.x)*(f.y)*(int)(this->field_x_width-1-f.x)*(int)(this->field_y_length-1-f.y);
            if(dist < 2 || dist > 4*this->speed || edge_root_func == 0){
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
        if(kv_name_grid.second.isFound() == false){//if not found and not base
            for(auto &p: kv_name_grid.second.getSignalBounds().getBounds()){
                signal_frontiers.insert(p);
                priority_frontiers.push_back(p);
                cv::circle(priority_img, p,2,cv::Scalar(255,255,0));
            }
        }
    }










    *this->output << "Total frontiers: " << "," << this->frontiers.size() << "," << " Prioritised frontiers: " << "," << priority_frontiers.size() << std::endl;
    
    
#ifdef SHOW_IMG
    cv::imshow("Filtered Frontiers", priority_img);
    cv::waitKey(WAITKEY_DELAY);
#endif
    

    this->costFunction(seen, priority_frontiers, signal_frontiers, best_point,best_score);

//COST FUNCTION


    
    if(best_score < -10000){ 
        // *this->output << "SHOULD BE DONE!" << std::endl;
        std::vector<cv::Point2i> missed_spots;

        cv::findNonZero(this->certainty_grids->at(BASE).getLikelihood(),missed_spots);

        if(missed_spots.size() == 0){ //fully explored
            // *this->output <<"DONE!" << std::endl;
            return std::make_pair(-1,-1);
        }

        this->costFunction(seen, reserve_frontiers, signal_frontiers, best_point,best_score);
        if(best_score < 1000){ 
            double missed_p_dist = this->field_x_width*this->field_x_width + this->field_y_length*this->field_y_length;
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
    cv::Mat best = this->certainty_grids->at(MAP).getLikelihood().clone();
    cv::circle(best, best_point, 3, cv::Scalar(255));
#ifdef SHOW_IMG
    cv::imshow("best", best);
    cv::waitKey(WAITKEY_DELAY);
#endif
    this->recordCommand(this->point2Pair(best_point));
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

    

    // *this->output << "Interpolation factor: " << factor << " Interpolated point: " << interpolated_pos.first << "," << interpolated_pos.second << std::endl;


    interpolated_pos.first = this->clipRange(0, this->field_x_width, interpolated_pos.first);
    interpolated_pos.second = this->clipRange(0, this->field_y_length, interpolated_pos.second);



    this->coords = interpolated_pos;
    // this->certainty_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
    
    Agent::step_counter++;

    return interpolated_pos;
}







std::pair<int,int> Agent::getCoords(){
    return this->coords;
}


cv::Mat Agent::getMap(){
    return this->certainty_grids->at(MAP).getLikelihood();
}

cv::Mat Agent::getSignalLocations() {
    cv::Mat confirmed = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC3);
    cv::Mat temp = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC3);
    for(auto &kv_name_grid: *(this->certainty_grids)){
        if(kv_name_grid.second.isFound() && kv_name_grid.first != BASE && kv_name_grid.first != MAP){
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

void Agent::logAgent() {
    
    *this->output << "Steps taken: " << "," << Agent::step_counter << std::endl;
    for(auto &kv_name_grid: *(this->certainty_grids)){
        if(kv_name_grid.first == BASE || kv_name_grid.first == MAP ) continue;
        *this->output << kv_name_grid.first << ","  << kv_name_grid.second.getSignalBounds().getCentre().x << " , " << kv_name_grid.second.getSignalBounds().getCentre().y << std::endl;
        *this->output << "Likelihood area: " << "," << kv_name_grid.second.getSignalBounds().getArea() << std::endl;
    }
}


