
#include <iostream>
#include "MatlabEngine.hpp"
#include "MatlabDataArray.hpp"

//   g++ -std=c++14 -I /usr/local/MATLAB/R2022a/extern/include/ -L /usr/local/MATLAB/R2022a/extern/bin/glnxa64 -pthread matlab_test.cpp -lMatlabDataArray -lMatlabEngine 



int main(void){

    using namespace matlab::engine;

    std::unique_ptr<MATLABEngine> matlabPtr = startMATLAB();
    matlab::data::ArrayFactory factory;
    std::vector<matlab::data::Array> args({
        factory.createArray<double>({ 1, 10 }, { 4, 8, 6, -1, -2, -3, -1, 3, 4, 5 }),
        factory.createScalar<int32_t>(3),
        factory.createCharArray("Endpoints"),
        factory.createCharArray("discard")
    });

    // Call MATLAB function with arguments and return results
    matlab::data::TypedArray<double> result = matlabPtr->feval(u"movsum", args);

    matlabPtr->eval(u"[X, Y] = meshgrid(-2:0.2:2);");
    matlabPtr->eval(u"Z = X .* exp(-X.^2 - Y.^2);");
    matlabPtr->eval(u"surf(Z)");
    
    matlabPtr->eval(u"print('SurfaceGraph', '-djpeg')");
    matlabPtr->eval(u"currentFolder = pwd;");
    std::cin.get();

    std::cout << "Hello world" << std::endl;

    // std::cout << result << std::endl;
    int i = 7;
}
