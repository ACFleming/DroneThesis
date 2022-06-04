#include <iostream>
#include <fstream>
#include <vector>





int main (int argc, char* argv[]){

    std::ofstream log_file;
    log_file.open("log_file.csv");

    log_file << "1,2,3,4, 5" ;
    log_file.close();
    std::cout << "DONE" << std::endl;
    return 0;
}