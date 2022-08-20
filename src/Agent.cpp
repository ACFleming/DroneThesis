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

Agent::Agent(std::string name, int x_coord, int y_coord, int field_width, int field_length, int scan_radius, int speed, std::map<std::string, Grid> *certainty_grids) {
    this->coords = std::make_pair(x_coord, y_coord);
    this->coord_history.push_back(this->coords);
    this->name = name;
    this->field_x_width = field_width;
    this->field_y_length = field_length;
    this->scan_radius = scan_radius;
    this->speed = speed;
    this->certainty_grids = certainty_grids;
    this->certainty_grids->insert(std::pair<std::string, Grid>(BASE,Grid(this->field_x_width, this->field_y_length) ));

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










void Agent::updateCertainty(Field f){



    //step 0 clear old measurements



    for(auto &kv_name_grid: *(this->certainty_grids)){
        kv_name_grid.second.prepareForUpdate(this->coords, this->scan_radius);
    }
    

    //step 1 get any signal measurements


    std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords); //to change for BT ping


    for(auto &m: measurements){

        if(this->certainty_grids->count(m.first) == 0){ //if no existing grid region
            this->certainty_grids->insert(std::pair<std::string, Grid> (m.first, Grid(m.first, this->certainty_grids->at(BASE).getLikelihood())));
            this->certainty_grids->at(m.first).prepareForUpdate(this->coords, this->scan_radius);
        }
        this->certainty_grids->at(m.first).receiveMeasurement(m.second);

    }

    for(auto &kv_name_grid: *(this->certainty_grids)){
        kv_name_grid.second.updateCertainty();
    }

    

}



std::pair<int,int> Agent::determineAction(){
 


    std::pair<int,int> next_position = this->coords;

    


    cv::Mat new_cells;
    std::vector<std::vector<cv::Point2i>> new_frontiers;

    double best_score = LONG_MIN;
    cv::Point2i best_point;



    cv::Mat seen;
    cv::threshold(this->certainty_grids->at(BASE).getLikelihood(), seen,unknown-1, unknown, cv::THRESH_BINARY_INV);

    this->frontiers = Grid::getImageFrontiers(seen);



    cv::imshow("seen", seen);
    cv::waitKey(WAITKEY_DELAY);




    //need to define hash function for cv::Point set

    struct point_hash {
    inline std::size_t operator()(const cv::Point2i & v) const {
        return v.x*31 + v.y;
        }
    };

    

    std::unordered_set<cv::Point,point_hash> signal_frontiers;

    std::vector<cv::Point> priority_frontiers;
    std::vector<cv::Point> reserve_frontiers;




    cv::Mat priority_img = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC3);    
    
    //filter frontier points based on distance
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

    //add signal bounds to the priority frontier list
    for(auto &kv_name_grid: *(this->certainty_grids)){
        if(kv_name_grid.second.isFound() == false && kv_name_grid.first != BASE){//if not found and not base
            for(auto &p: kv_name_grid.second.getSignalBounds().getBounds()){
                signal_frontiers.insert(p);
                priority_frontiers.push_back(p);
                cv::circle(priority_img, p,2,cv::Scalar(255,255,0));
            }
        }
    }










    std::cout << "total frontiers: " << this->frontiers.size() << " prioritised frontiers: " << priority_frontiers.size() << std::endl;
    
    

    cv::imshow("Filtered Frontiers", priority_img);

    // cv::waitKey(0);





    for(auto &f: priority_frontiers ){
#ifdef COST_VEC_PRINT
        std::cout << "Point: " << f.x << "," << f.y;
#endif
        


        double score = 0;

        if(std::find(this->coord_history.begin(), this->coord_history.end(), this->point2Pair(f)) != this->coord_history.end()){
            double repeat_point_mod = -100000;
            score += repeat_point_mod;
            std::cout << " Repeat point!";
        }
        

        if(signal_frontiers.count(f) > 0 ){ //if its a signal frontier point;
            score += 1000;
        }



        double dist = this->dist(this->coords, this->point2Pair(f));
        if(dist < 0.1*this->speed) {
            score = LONG_MIN;
        }

        double dist_mod = -10;
        

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




        new_frontiers = Grid::getImageFrontiers(new_cells);

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




        std::vector<std::vector<cv::Point>> inverse_frontiers = Grid::getImageFrontiers(inverse);


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









#ifdef COST_VEC_PRINT
        std::cout  << " Final Score: " << score <<  std::endl;  
#endif      
        if(score > best_score){
            best_score = score;
            best_point = f;
        }


        // cv::waitKey(0);
        
    }

    if(best_score < -90000){ //repeat point
        std::cout << "SHOULD BE DONE!" << std::endl;
        std::vector<cv::Point2i> missed_spots;

        cv::findNonZero(this->certainty_grids->at(BASE).getLikelihood(),missed_spots);

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




    std::cout <<  "BEST! " << "Point:" << best_point.x <<"," << best_point.y << " Score: " << best_score << std::endl;
    cv::Mat best = this->certainty_grids->at(BASE).getLikelihood().clone();
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







std::pair<int,int> Agent::getCoords(){
    return this->coords;
}


cv::Mat Agent::getCertGrid(){
    return this->certainty_grids->at(BASE).getLikelihood();
}
