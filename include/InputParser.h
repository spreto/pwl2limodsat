#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <fstream>
#include <string>

#include "LinearPiece.h"
#include "PiecewiseLinearFunction.h"

using namespace std;

enum ExecutionMode { PWL, TL };

class InputParser
{
    public:
        InputParser(const char* iFN);
        virtual ~InputParser();

        ExecutionMode executionMode() { return mode; }

        LinearPiece createTLInstance();
        PiecewiseLinearFunction createPWLInstance();

    protected:

    private:
        string inputFileName;
        ifstream inputFile;
        string currentLine;
        ExecutionMode mode;

        void nextLine();
        vector<LinearPieceCoefficient> readLinearPiece(unsigned beginingPosition);
};

#endif // INPUTPARSER_H
