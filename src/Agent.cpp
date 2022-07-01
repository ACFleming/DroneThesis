#include "Agent.hpp"



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
    
    


    this->occupancy_grid = cv::Mat::zeros(this->field_y_length, this->field_x_width, CV_8UC1);

        
    // // std::cout << this->occupancy_grid << std::endl;

    // cv::Rect boundary(0,0, this->field_x_width-1, this->field_y_length-1);

    // cv::rectangle(this->occupancy_grid, boundary, cv::Scalar(blocked, blocked, blocked));


    // std::cout << this->occupancy_grid.depth() << std::endl;

    this->occupancy_grid.at<uint8_t>(this->coords.second,this->coords.first) = scanned;




    



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


std::vector<cv::Point2i> Agent::getOctagonPoints(int x, int y){
    std::vector<cv::Point2i> clockwise_points;      
    cv::Point2i p;
    //N
    p = cv::Point( this->clipRange(0, this->field_x_width, int(x))                                ,this->clipRange(0, this->field_y_length, int(y - this->scan_radius)));
    clockwise_points.push_back(p);
    //NE
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x + sin(PI/4)*this->scan_radius))  ,this->clipRange(0, this->field_y_length, int(y - sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
    //E
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x + this->scan_radius))             ,this->clipRange(0, this->field_y_length, int(y)));
    clockwise_points.push_back(p);
    //SE
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x + sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y + sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
    //S
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x))                                 ,this->clipRange(0, this->field_y_length, int(y + this->scan_radius)));
    clockwise_points.push_back(p);
    //SW
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x - sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y + sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
    //W
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x - this->scan_radius))             ,this->clipRange(0, this->field_y_length, int(y)));
    clockwise_points.push_back(p);
    //NW
    p = cv::Point(this->clipRange(0, this->field_x_width, int(x - sin(PI/4)*this->scan_radius))   ,this->clipRange(0, this->field_y_length, int(y - sin(PI/4)*this->scan_radius)));
    clockwise_points.push_back(p);
    return clockwise_points;

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

void Agent::updateScannedGrid(){

    cv::Mat mask = this->rangeMask(this->coords.first, this->coords.second, scanned);

    cv::bitwise_or(this->occupancy_grid, mask, this->occupancy_grid);



    this->frontiers = this->getNewFrontiers(this->coords.first, this->coords.second);
    std::cout << "Frontier count: " << this->frontiers.size() << std::endl; 




}

std::vector<std::vector<cv::Point2i>> Agent::getNewFrontiers(int x, int y){

    // cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);

    // for(auto &v: this->frontiers){
    //     this->drawArc(frontier_map, this->point2Pair(v[0]), this->point2Pair(v[1]), this->point2Pair(v[2]));

    // }

    // this->drawArc(frontier_map, std::make_pair(40,10), std::make_pair(89,0), std::make_pair(0,40));

    // cv::rectangle(frontier_map,cv::Point2i(0,0), cv::Point2i(this->field_x_width-1, this->field_y_length-1),cv::Scalar(255));

    // cv::imshow("frontier_map", frontier_map);
    // cv::waitKey(0);





    // cv::Mat new_frontier = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);    

    // cv::circle(new_frontier,cv::Point2i(x,y),this->scan_radius, cv::Scalar(255));





    // cv::imshow("new_frontier", new_frontier);
    // cv::waitKey(0);


    // cv::bitwise_or(frontier_map,new_frontier,frontier_map);

    
    // // cv::findNonZero()
    
    // cv::imshow("frontier_map", frontier_map);
    // cv::waitKey(0);


    // std::vector<std::vector<cv::Point2i>> found_contours;
    // // std::vector<cv::Vec4i> hierarchy;
    // // cv::findContours(frontier_map,found_contours, hierarchy, cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);



    // // cv::drawContours(frontier_map, found_contours, -1, cv::Scalar(255));
    


    // return found_contours;















    // // std::vector<cv::Point2i> oct = this->getOctagonPoints(x, y);



    // // cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);
    
    // // std::vector<std::vector<cv::Point2i>> new_frontiers;
    // // new_frontiers.push_back(oct);
    // // // if(!this->frontiers.empty()) new_frontiers.push_back(this->frontiers);
    // // for(auto &f: this->frontiers){
    // //     new_frontiers.push_back(f);
    // // }

    
    // // cv::drawContours(frontier_map, new_frontiers, -1, cv::Scalar(255));
    
    // // // cv::imshow("Frontier map", frontier_map);
    // // // cv::waitKey(0);


    
    // std::vector<std::vector<cv::Point2i>> found_contours;
    // // std::vector<cv::Vec4i> hierarchy;
    // // cv::findContours(frontier_map,found_contours, hierarchy, cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);

    // // //filter out non-external contours

    // // // std::vector<std::vector<cv::Point2i>> ext_contours;

    // // // for(auto i = 0; i < hierarchy.size(); i++){
    // // //     if(hierarchy[i][3] == -1){
    // // //         ext_contours.push_back(found_contours[i]);
    // // //     }
    // // // }





    // // std::vector<std::vector<cv::Point2i>> approx;
    // // // for(auto &e: ext_contours){
    // // std::vector<cv::Point2i> a;
    // // cv::approxPolyDP(found_contours[0], a, 1, true);
    // // approx.push_back(a);
    
    
    // // return approx;
    // // //OCC CONTOURS

    cv::Mat occ_frontier = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);



    cv::Mat mask = this->rangeMask(x, y, scanned);

    cv::bitwise_or(this->occupancy_grid, mask, occ_frontier);

    // cv::rectangle(occ_frontier, cv::Point2i(0,0), cv::Point2i(this->field_x_width, this->field_y_length),cv::Scalar(0));

    // cv::Canny(occ_frontier,occ_frontier,100,200);
    // cv::imshow("OCC Frontier map", occ_frontier);
    // cv::waitKey(0);

    cv::Mat ctr_img = occ_frontier;

    // cv::threshold(occ_frontier,ctr_img,scanned-1, 255,cv::THRESH_BINARY_INV);
    // cv::imshow("ctr_img", ctr_img);
    // cv::waitKey(0);


    //bonus

//    mask = this->rangeMask(x+40, y+40, scanned);

//     cv::bitwise_or(this->occupancy_grid, mask, occ_frontier);

    // cv::Mat ctr_image;
    // cv::copyMakeBorder(occ_frontier, ctr_image, 1,1,1,1,cv::BORDER_CONSTANT, 0);

    // cv::imshow("ctr_image", ctr_image);
    // cv::waitKey(0);



    std::vector<std::vector<cv::Point2i>> occ_contours;
    std::vector<cv::Vec4i> occ_hierarchy;
    cv::findContours(ctr_img,occ_contours, occ_hierarchy, cv::RETR_LIST,cv::CHAIN_APPROX_NONE, cv::Point(0,0));

    

    cv::drawContours(occ_frontier, occ_contours, -1, cv::Scalar(255));
    
    // cv::imshow("OCC Frontier map", occ_frontier);
    // cv::waitKey(0);

    //filter out non-external contours



    // for(auto i = 0; i < occ_hierarchy.size(); i++){
    //     if(occ_hierarchy[i][3] != -1){
    //         ext_occ_contours.push_back(ext_occ_contours[i]);
    //     }
    // }

    




    std::vector<std::vector<cv::Point2i>> occ_approx;
    for(auto &e: occ_contours){
        std::vector<cv::Point2i> a;
        cv::approxPolyDP(e, a, 1, true);
        occ_approx.push_back(a);
    }

    // cv::drawContours(occ_frontier, occ_approx, -1, cv::Scalar(255));
    
    // cv::imshow("OCC approx map", occ_frontier);
    // cv::waitKey(10);





    

    
    return occ_approx;

}

std::pair<int,int> Agent::determineAction(){


    //Rastering Assumption
    //Range of scan >= speed

    


    std::pair<int,int> next_position = this->coords;

    

    
    std::vector<std::pair<cv::Point2i,double>> cost_vec;

    int curr_scanned = cv::countNonZero(this->occupancy_grid);
    int curr_frontier_count = this->frontiers[0].size();

    // std::vector<cv::Point2i> frontiers = this->getNewFrontiers(this->coords.first, this);
    cv::Mat new_cells;
    std::vector<std::vector<cv::Point2i>> new_frontiers;

    double best_score = DBL_MIN;
    cv::Point2i best_point;

    for(auto &f: this->frontiers[0] ){
        double score = 0;

        if(std::find(this->coord_history.begin(), this->coord_history.end(), this->point2Pair(f)) != this->coord_history.end()){
            score = -10000;
        }
        




        int dist = int(hypot(this->coords.first - f.x, this->coords.second-f.y));
        if(dist == 0) continue;



        score -= 1*dist;


        cv::bitwise_or(this->occupancy_grid, this->rangeMask(f.x, f.y, 200), new_cells);    
        int new_scanned_count = cv::countNonZero(new_cells) - curr_scanned;

        score += 0.01*new_scanned_count;
        
        // cv::imshow("New cells", new_cells);
        // cv::waitKey(0);



        new_frontiers = this->getNewFrontiers(f.x, f.y);
        int new_frontier_count = new_frontiers.size() - curr_frontier_count;

        score -= 10*new_frontier_count;


        cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);

        cv::drawContours(frontier_map, new_frontiers, -1, cv::Scalar(255));
        // cv::imshow("New frontiers", frontier_map);
        // cv::waitKey(0);

        double ctr_area = cv::contourArea(new_frontiers[0]);
        std::vector<cv::Point2i> hull_points;

        cv::convexHull(new_frontiers[0], hull_points);
        double hull_area = cv::contourArea(hull_points);
        if(hull_area == 0){
            std::cout << "HUH?" << std::endl;
            hull_area = cv::contourArea(hull_points);
        }

        double solidity = ctr_area/hull_area;

        score += 50 * 100*solidity;


        cv::Mat hull_img = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);
        std::vector<std::vector<cv::Point2i>> h;
        h.push_back(hull_points);
        cv::drawContours(new_cells, h, -1, cv::Scalar(255));

        cv::imshow("hull_img", new_cells);
        cv::waitKey(10);

  

        std::cout << "Point: " << f.x << "," << f.y << " NSC: " << new_scanned_count << " NFC: " << new_frontier_count << " D: " << dist << " SOL: " << solidity << " Score: " << score <<  std::endl;

        
        if(score > best_score){
            best_score = score;
            best_point = f;
        }
        
    }


    // //Finf min arg
    // std::pair<cv::Point2i,double>result = *std::max_element(cost_vec.cbegin(), cost_vec.cend(), [](const std::pair<cv::Point2i,double> &lhs, const std::pair<cv::Point2i,double> &rhs) {
    //     return lhs.second < rhs.second;    
    // });


    std::cout <<  "BEST! " << "Point:" << best_point.x <<"," << best_point.y << " Score: " << best_point << std::endl;
    cv::waitKey(0);


    return this->point2Pair(best_point);


}

void Agent::moveToPosition(std::pair<int,int> pos){
    this->occupancy_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
    pos.first = this->clipRange(0, this->field_x_width, pos.first);
    pos.second = this->clipRange(0, this->field_y_length, pos.second);

    std::pair<int,int> interpolated_pos = pos;
    double factor = this->speed/this->dist(pos, this->coords);
    if(factor < 1){ // i.e. if dist point is furhter than the speed of one step
        
        interpolated_pos.first = (pos.first-this->coords.first)*factor + this->coords.first;
        interpolated_pos.second = (pos.second-this->coords.second)*factor + this->coords.second;
    }



    this->coords = interpolated_pos;
    this->occupancy_grid.at<uint8_t>(this->coords.second, this->coords.first) = scanned;
    this->coord_history.push_back(this->coords);
}



void Agent::measureSignalStrength(Field f) {
    std::vector<std::pair<std::string, double>> measurements = f.getMeasurements(this->coords);
    // for(auto &m: measurements){
    //     std::cout << m.first << ": " << m.second << std::endl;
    // }
    this->measurements.push_back(measurements);
    
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


void Agent::showOccGrid(){
    std::cout << "Occupancy Grid" << std::endl;
    std::cout << this->occupancy_grid << std::endl;

}

cv::Mat Agent::getOccGrid(){
    return this->occupancy_grid;
}

cv::Mat Agent::getFrontierMap(){
    cv::Mat frontier_map = cv::Mat::zeros(this->field_y_length,this->field_x_width,CV_8UC1);



    cv::drawContours(frontier_map, this->frontiers  , -1, cv::Scalar(255));
    
    return frontier_map;
}