#include "SignalSource.hpp"


SignalSource::SignalSource(int field_width, int field_length) {

    int x_max = 0.99*field_width-1;
    int x_min = 0.01*field_width;
    int y_max = 0.99*field_length-1;
    int y_min = 0.01*field_length;

    // min + rand() % (( max + 1 ) - min);

    Zone z = Zone(field_width, field_length);


    do
    {
        this->x_coord = x_min + std::rand()%((x_max - x_min + 1));
        this->y_coord = y_min + std::rand()%((y_max - y_min + 1));;
        this->id = this->randomString(6);
    } while (!z.inZone(cv::Point2i(x_coord, y_coord)));
    

    // std::cout << "ID: " << this->id << " X: " << this->x_coord << " Y: " << this->y_coord << std::endl;

}

SignalSource::SignalSource(int x, int y, std::string id) {
    this->x_coord = x;
    this->y_coord = y;
    this->id = id;
    
}

SignalSource::~SignalSource() {
    
}

std::pair<int,int> SignalSource::getCoords() {
    return std::make_pair(this->x_coord, this->y_coord);
}

std::string SignalSource::getId() {
    return this->id;
}


std::string SignalSource::randomString(std::size_t length)
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - (size_t)1);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}