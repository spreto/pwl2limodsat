#include "VariableManager.h"

VariableManager::VariableManager(unsigned dim) :
    counter(dim) {}

unsigned VariableManager::currentVariable()
{
    return counter;
}

unsigned VariableManager::newVariable()
{
    counter++;

    return counter;
}

unsigned VariableManager::zeroVariable()
{
    if ( zero == 0 )
        zero = ++counter;

    return zero;
}

bool VariableManager::isThereConstant(unsigned denum)
{
    if ( constantsMap.find( denum ) != constantsMap.end() )
        return true;
    else
        return false;
}

Variable VariableManager::constant(unsigned denum)
{
    return constantsMap.find(denum)->second;
}

Variable VariableManager::newConstant(unsigned denum)
{
    constantsMap.insert(pair<unsigned,Variable>(denum,newVariable()));
    return currentVariable();
}

bool VariableManager::isThereAuxMultVariable(unsigned denum)
{
    if ( auxMultMap.find(denum) != auxMultMap.end() )
        return true;
    else
        return false;
}

Variable VariableManager::auxMultVariable(unsigned denum)
{
    if ( auxMultMap.find(denum) != auxMultMap.end() )
        return auxMultMap.find(denum)->second;
    else
    {
        auxMultMap.insert(pair<unsigned,Variable>(denum,newVariable()));
        return currentVariable();
    }
}
