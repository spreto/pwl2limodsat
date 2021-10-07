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
        Variable currentVariable();
        Variable newVariable();
        Variable zeroVariable();
        bool isThereConstant(unsigned denum);
        Variable constant(unsigned denum);
        Variable newConstant(unsigned denum);
        bool isThereAuxMultVariable(unsigned denum);
        Variable auxMultVariable(unsigned denum);

    protected:

    private:
        unsigned counter;
        Variable zero = 0;
        bool zeroDefined = false;
        bool counterInitialized = false;
        std::map<unsigned,Variable> constantsMap;
        std::map<unsigned,Variable> auxMultMap;
};
}

#endif // VARIABLEMANAGER_H
