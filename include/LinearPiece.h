#ifndef LINEARPIECE_H
#define LINEARPIECE_H

#include <vector>
#include <string>
#include <fstream>
#include "VariableManager.h"
#include "Formula.h"

using namespace lukaFormula;

namespace pwl2limodsat
{
class LinearPiece
{
    public:
        LinearPiece(const LinearPieceData& data, VariableManager *varMan);
        LinearPiece(const LinearPieceData& data, std::string inputFileName);
        ~LinearPiece();

        void representModsat();
        Modsat getRepresentationModsat();
        void printModsatSetAs(std::ofstream *output, std::string intro);
        void printModsatSet(std::ofstream *output);
        void printLimodsatFile();

        static Formula zeroFormula(VariableManager *var);
        template<class T> static Modsat binaryModsat(VariableManager *var, unsigned n, const T& logTerm);
        static ModsatSet defineConstant(VariableManager *var, unsigned denum);
        static Modsat multiplyConstant(VariableManager *var, unsigned num, unsigned denum);

    protected:
        LinearPieceData linearPieceData;
        unsigned dim;

    private:
        std::string outputFileName;

        Modsat representationModsat;

        VariableManager *var;
        bool ownVariableManager = false;
        bool modsatTranslation = false;

        Formula zeroFormula();
        template<class T> Modsat binaryModsat(unsigned n, const T& logTerm);
        ModsatSet defineConstant(unsigned denum);
        Modsat multiplyConstant(unsigned num, unsigned denum);
        Formula variableSecondMultiplication(unsigned n, Variable var);
        void pwl2limodsat();
};
}

#endif // LINEARPIECE_H
