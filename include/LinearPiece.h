#ifndef LINEARPIECE_H
#define LINEARPIECE_H

#include <vector>
#include <string>
#include <fstream>

#include "VariableManager.h"
#include "Formula.h"

using namespace std;

typedef pair<int,unsigned> LinearPieceCoefficient;

class LinearPiece
{
    public:
        LinearPiece(const vector<LinearPieceCoefficient>& coefs, VariableManager *varMan);
        LinearPiece(const vector<LinearPieceCoefficient>& coefs, string inputFileName);
        ~LinearPiece();

        void representModSat();
        ModSat getRepresentationModSat();
        void printModSatSet(ofstream *output);
        void printRepresentation();

    protected:

    private:
        string outputFileName;
        vector<LinearPieceCoefficient> pieceCoefficients;
        unsigned dim;
        VariableManager *var;
        bool ownVariableManager = false;
        ModSat representationModSat;

        Formula zeroFormula();
        template<class T> ModSat binaryModSat(unsigned n, const T& logTerm);
        ModSatSet defineConstant(unsigned denum);
        ModSat multiplyConstant(unsigned num, unsigned denum);
        Formula variableSecondMultiplication(unsigned n, Variable var);
};

#endif // LINEARPIECE_H
