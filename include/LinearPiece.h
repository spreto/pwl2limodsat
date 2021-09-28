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
        vector<LinearPieceCoefficient> pieceCoefficients;
        unsigned dim;

    private:
        string outputFileName;

        ModSat representationModSat;

        VariableManager *var;
        bool ownVariableManager = false;
        bool modsatTranslation = false;

        Formula zeroFormula();
        template<class T> ModSat binaryModSat(unsigned n, const T& logTerm);
        ModSatSet defineConstant(unsigned denum);
        ModSat multiplyConstant(unsigned num, unsigned denum);
        Formula variableSecondMultiplication(unsigned n, Variable var);
        void pwl2limodsat();
};

#endif // LINEARPIECE_H
