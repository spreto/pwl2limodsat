#ifndef VARIABLEMANAGER_H
#define VARIABLEMANAGER_H

#include <map>
#include "pwl2limodsat.h"

namespace pwl2limodsat
{
class VariableManager
{
    public:
        VariableManager(unsigned dim);
        VariableManager();
        void setDimension(unsigned dim);
        void jumpToVariable(Variable toVar);
        Variable currentVariable();
        Variable newVariable();
        Variable zeroVariable();
        bool isThereConstant(LPCoefNonNegative denum);
        Variable constant(LPCoefNonNegative denum);
        Variable newConstant(LPCoefNonNegative denum);
        bool isThereAuxMultVariable(LPCoefNonNegative denum);
        Variable auxMultVariable(LPCoefNonNegative denum);

    protected:

    private:
        Variable counter;
        Variable zero = 0;
        bool zeroDefined = false;
        bool counterInitialized = false;
        std::map<LPCoefNonNegative,Variable> constantsMap;
        std::map<LPCoefNonNegative,Variable> auxMultMap;

        void verifyInitialization();
};
}

#endif // VARIABLEMANAGER_H
