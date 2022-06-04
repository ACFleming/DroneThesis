#include <string>
#include <iostream>
#include <random>

class SignalSource
{
private:
    int x_coord;
    int y_coord;
    std::string id;
    std::string randomString(std::size_t length);

public:
    SignalSource(int x, int y, std::string id);
    SignalSource(int field_width, int field_length);
    ~SignalSource();

    std::pair<int,int> getCoords();
    std::string getId();
};


