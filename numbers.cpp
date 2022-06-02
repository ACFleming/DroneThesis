#include "MatlabDataArray.hpp"
#include "MatlabEngine.hpp"
#include <iostream>
#include <memory>

int main() {
    // Evaluate functions in MATLAB

    using namespace matlab::engine;

    // Start MATLAB engine synchronously
    std::unique_ptr<MATLABEngine> matlabPtr = startMATLAB();

    // Evaluate commands in MATLAB
    matlabPtr->eval(u"[X, Y] = meshgrid(-2:0.2:2);");
    matlabPtr->eval(u"Z = X .* exp(-X.^2 - Y.^2);");
    matlabPtr->eval(u"surf(Z)");
    matlabPtr->eval(u"print('SurfaceGraph', '-djpeg')");
    matlabPtr->eval(u"currentFolder = pwd;");

    // Get the name of the folder containing the jpeg file
    matlab::data::CharArray currentFolder = matlabPtr->getVariable(u"currentFolder");
    std::cout << "SurfaceGraph.jpg written to this folder: " << 
          currentFolder.toAscii() << std::endl;
}