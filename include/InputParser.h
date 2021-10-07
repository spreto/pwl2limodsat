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
        InputParser(const char* inputPwlFileName);
        virtual ~InputParser();
        ExecutionMode executionMode() { return mode; }
        LinearPieceData getTlInstanceData();
        PiecewiseLinearFunctionData getPwlInstanceData();
        BoundaryPrototypeCollection getPwlInstanceBoundProt();

    protected:

    private:
        std::string pwlFileName;
        std::ifstream pwlFile;
        std::string currentLine;
        ExecutionMode mode;

        PiecewiseLinearFunctionData pwlData;
        BoundaryPrototypeCollection boundaryPrototypeData;
        bool pwlTranslation = false;

        void nextLine();
        LinearPieceData readLinearPiece(unsigned beginingPosition);
        void buildPwlInstance();
};
}

#endif // INPUTPARSER_H
