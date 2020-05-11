#ifndef VARIABLEMANAGER_H
#define VARIABLEMANAGER_H

#include <map>

using namespace std;

typedef unsigned Variable;

class VariableManager
{
    public:
        VariableManager(unsigned dim);
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
        map<unsigned,Variable> constantsMap;
        map<unsigned,Variable> auxMultMap;
};

#endif // VARIABLEMANAGER_H
