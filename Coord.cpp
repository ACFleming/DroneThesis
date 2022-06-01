#include "Coord.hpp"

Coord::Coord(int x, int y, std::string id) {
    this->x = x;
    this->y = y;
    this->id = "Coordinate";
}

Coord::Coord(int x, int y, std::string id){
    this->x = x;
    this->y = y;
    this->id = id;
}


float Coord::dist2Point(Coord first, Coord second) {

    return sqrt((first.x-second.x)^2 + (first.y-second.y)^2);    
}





Coord::~Coord() {
    
}
