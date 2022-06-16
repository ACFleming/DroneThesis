#include "SignalSource.hpp"


SignalSource::SignalSource(int field_width, int field_length) {

    this->x_coord= std::rand() % field_width;
    this->y_coord = std::rand() % field_length;
    this->id = this->randomString(8);
    std::cout << "ID: " << this->id << " X: " << this->x_coord << " Y: " << this->y_coord << std::endl;

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
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

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