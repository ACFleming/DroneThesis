#pragma once


#include <string>
#include <math.h>

class Coord
{
private:
    int x;
    int y;
    std::string id;

    
public:
    Coord(int x, int y);
    Coord(int x, int y, std::string id);
    ~Coord();
    float dist2Point(Coord first, Coord second);
};


