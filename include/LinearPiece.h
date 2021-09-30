#ifndef LINEARPIECE_H
#define LINEARPIECE_H

#include <vector>
#include <string>
#include <fstream>
#include "VariableManager.h"
#include "Formula.h"

namespace pwl2limodsat
{
class LinearPiece
{
    public:
        LinearPiece(const LinearPieceData& data, VariableManager *varMan);
        LinearPiece(const LinearPieceData& data, std::string inputFileName);
        ~LinearPiece();

        void representModSat();
        ModSat getRepresentationModSat();
        void printModSatSet(std::ofstream *output);
        void printRepresentation();

    protected:
        LinearPieceData linearPieceData;
        unsigned dim;

    private:
        std::string outputFileName;

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
}

#endif // LINEARPIECE_H
