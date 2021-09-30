#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <fstream>
#include <string>

#include "LinearPiece.h"
#include "PiecewiseLinearFunction.h"

namespace pwl2limodsat
{
class InputParser
{
    public:
        InputParser(const char* iFN);
        virtual ~InputParser();
        ExecutionMode executionMode() { return mode; }
        LinearPieceData getTLInstanceData();
        PiecewiseLinearFunctionData getPWLInstanceData();
        BoundaryPrototypeCollection getPWLInstanceBoundProt();

    protected:

    private:
        std::string inputFileName;
        std::ifstream inputFile;
        std::string currentLine;
        ExecutionMode mode;

        PiecewiseLinearFunctionData pwlInstanceData;
        BoundaryPrototypeCollection pwlInstanceBoundProt;
        bool pwlTranslation = false;

        void nextLine();
        LinearPieceData readLinearPiece(unsigned beginingPosition);
        void buildPWLInstance();
};
}

#endif // INPUTPARSER_H
