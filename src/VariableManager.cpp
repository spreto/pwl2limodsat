#include "VariableManager.h"

namespace pwl2limodsat
{
VariableManager::VariableManager(unsigned dim) :
    counter(dim)
{
    counterInitialized = true;
}

void VariableManager::setDimension(unsigned dim)
{
    if ( !counterInitialized )
    {
        counter = dim;
        counterInitialized = true;
    }
    else
        throw std::invalid_argument("Dimension already defined.");
}

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

bool VariableManager::isThereConstant(LPCoefNonNegative denum)
{
    if ( constantsMap.find( denum ) != constantsMap.end() )
        return true;
    else
        return false;
}

Variable VariableManager::constant(LPCoefNonNegative denum)
{
    return constantsMap.find(denum)->second;
}

Variable VariableManager::newConstant(LPCoefNonNegative denum)
{
    constantsMap.insert(std::pair<LPCoefNonNegative,Variable>(denum,newVariable()));
    return currentVariable();
}

bool VariableManager::isThereAuxMultVariable(LPCoefNonNegative denum)
{
    if ( auxMultMap.find(denum) != auxMultMap.end() )
        return true;
    else
        return false;
}

Variable VariableManager::auxMultVariable(LPCoefNonNegative denum)
{
    if ( auxMultMap.find(denum) != auxMultMap.end() )
        return auxMultMap.find(denum)->second;
    else
    {
        auxMultMap.insert(std::pair<LPCoefNonNegative,Variable>(denum,newVariable()));
        return currentVariable();
    }
}
}
