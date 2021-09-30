#include <iostream>

#include "InputParser.h"

int main(int argc, char **argv)
{
    pwl2limodsat::InputParser parser(argv[1]);
    std::cout << "Input file opened successfully." << std::endl;

    if ( parser.executionMode() == pwl2limodsat::TL )
    {
        pwl2limodsat::LinearPiece instance(parser.getTlInstanceData(),argv[1]);
        std::cout << "Truncated Linear Function instantiated." << std::endl;
        std::cout << "Building representation... ";
        instance.printRepresentation();
        std::cout << "Done!" << std::endl;
    }
    else if ( parser.executionMode() == pwl2limodsat::PWL )
    {
        pwl2limodsat::PiecewiseLinearFunction instance(parser.getPwlInstanceData(),parser.getPwlInstanceBoundProt(),argv[1]);
        std::cout << "Piecewise Linear Function instantiated." << std::endl;
        std::cout << "Building representation... ";
        instance.printRepresentation();
        std::cout << "Done!" << std::endl;
    }

    return 0;
}
