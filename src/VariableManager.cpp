#include <stdexcept>
#include "VariableManager.h"

namespace pwl2limodsat
{
VariableManager::VariableManager(unsigned dim) :
    counter(dim)
{
    counterInitialized = true;
}

VariableManager::VariableManager() {};

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

void VariableManager::verifyInitialization()
{
    if ( !counterInitialized )
        throw std::invalid_argument("Variable Manager dimension still not set.");
}

void VariableManager::jumpToVariable(Variable toVar)
{
    verifyInitialization();

    if ( toVar >= counter )
        counter = toVar;
    else
        throw std::invalid_argument("Cannot jump to smaller variable number.");
}

unsigned VariableManager::currentVariable()
{
    verifyInitialization();

    return counter;
}

unsigned VariableManager::newVariable()
{
    verifyInitialization();

    counter++;

    return counter;
}

unsigned VariableManager::zeroVariable()
{
    verifyInitialization();

    if ( zero == 0 )
        zero = ++counter;

    return zero;
}

bool VariableManager::isThereConstant(LPCoefNonNegative denum)
{
    verifyInitialization();

    if ( constantsMap.find( denum ) != constantsMap.end() )
        return true;
    else
        return false;
}

Variable VariableManager::constant(LPCoefNonNegative denum)
{
    verifyInitialization();

    return constantsMap.find(denum)->second;
}

Variable VariableManager::newConstant(LPCoefNonNegative denum)
{
    verifyInitialization();

    constantsMap.insert(std::pair<LPCoefNonNegative,Variable>(denum,newVariable()));
    return currentVariable();
}

bool VariableManager::isThereAuxMultVariable(LPCoefNonNegative denum)
{
    verifyInitialization();

    if ( auxMultMap.find(denum) != auxMultMap.end() )
        return true;
    else
        return false;
}

Variable VariableManager::auxMultVariable(LPCoefNonNegative denum)
{
    verifyInitialization();

    if ( auxMultMap.find(denum) != auxMultMap.end() )
        return auxMultMap.find(denum)->second;
    else
    {
        auxMultMap.insert(std::pair<LPCoefNonNegative,Variable>(denum,newVariable()));
        return currentVariable();
    }
}
}
