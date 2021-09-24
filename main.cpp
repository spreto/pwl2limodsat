#include <iostream>

#include "InputParser.h"

using namespace std;

int main(int argc, char **argv)
{
    InputParser parser(argv[1]);
    cout << "Input file opened successfully." << endl;

    if ( parser.executionMode() == TL )
    {
        LinearPiece instance = parser.createTLInstance();
        cout << "Truncated Linear Function instantiated." << endl << endl;
        cout << "Building representation... ";
        instance.representModSat();
        instance.printRepresentation();
        cout << "Done!" << endl << endl;
    }
    else if ( parser.executionMode() == PWL )
    {
        PiecewiseLinearFunction instance = parser.createPWLInstance();
        cout << "Piecewise Linear Function instantiated." << endl << endl;
        cout << "Building representation... ";
        instance.representModSat();
        instance.printRepresentation();
        cout << "Done!" << endl << endl;
    }

    return 0;
}
