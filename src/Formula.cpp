#include <stdexcept>

#include "Formula.h"

namespace lukaFormula
{
Formula::Formula()
{
    emptyFormula = true;
}

Formula::Formula(const Clause& clau)
{
    unitClauses.push_back(UnitClause(++unitCounter, clau));
}

Formula::Formula(Literal lit)
{
    unitClauses.push_back(UnitClause(++unitCounter, Clause(1,lit)));
}

Formula::Formula(pwl2limodsat::Variable var) :
    Formula((Literal) var) {}

Formula::Formula(const Formula& form, LogicalSymbol unSym) :
    Formula(form)
{
    if ( unSym == Neg )
        this->negateFormula();
    else
        throw std::invalid_argument("Not a valid logic operation.");
}

Formula::Formula(const Formula& form1, const Formula& form2, LogicalSymbol binSym) :
    Formula(form1)
{
    if ( binSym == Lor )
        this->addLukaDisjunction(form2);
    else if ( binSym == Equiv )
        this->addEquivalence(form2);
    else if ( binSym == Impl )
        this->addImplication(form2);
    else if ( binSym == Max )
        this->addMaximum(form2);
    else if ( binSym == Min )
        this->addMinimum(form2);
    else
        throw std::invalid_argument("Not a valid logic operation.");
}

void Formula::negateFormula()
{
    negations.push_back(Negation(unitCounter+1, unitCounter));

    unitCounter++;
}

void Formula::addUnits(const Formula& form)
{
    for ( size_t i = 0; i < form.getUnitClauses().size(); i++ )
        unitClauses.push_back(UnitClause(form.getUnitClauses().at(i).first + unitCounter,
                              form.getUnitClauses().at(i).second));

    for ( size_t i = 0; i < form.getNegations().size(); i++ )
        negations.push_back(Negation(form.getNegations().at(i).first + unitCounter,
                            form.getNegations().at(i).second + unitCounter));

    for ( size_t i = 0; i < form.getLDisjunctions().size(); i++ )
        lDisjunctions.push_back( LDisjunction(std::get<0>(form.getLDisjunctions().at(i)) + unitCounter,
                                              std::get<1>(form.getLDisjunctions().at(i)) + unitCounter,
                                              std::get<2>(form.getLDisjunctions()[i]) + unitCounter) );

    for ( size_t i = 0; i < form.getEquivalences().size(); i++ )
        equivalences.push_back( Equivalence(std::get<0>(form.getEquivalences().at(i)) + unitCounter,
                                            std::get<1>(form.getEquivalences().at(i)) + unitCounter,
                                            std::get<2>(form.getEquivalences()[i]) + unitCounter) );

    for ( size_t i = 0; i < form.getImplications().size(); i++ )
        implications.push_back( Implication(std::get<0>(form.getImplications().at(i)) + unitCounter,
                                            std::get<1>(form.getImplications().at(i)) + unitCounter,
                                            std::get<2>(form.getImplications()[i]) + unitCounter) );

    for ( size_t i = 0; i < form.getMaximums().size(); i++ )
        maximums.push_back( Maximum(std::get<0>(form.getMaximums().at(i)) + unitCounter,
                                    std::get<1>(form.getMaximums().at(i)) + unitCounter,
                                    std::get<2>(form.getMaximums()[i]) + unitCounter) );

    for ( size_t i = 0; i < form.getMinimums().size(); i++ )
        minimums.push_back( Minimum(std::get<0>(form.getMinimums().at(i)) + unitCounter,
                                    std::get<1>(form.getMinimums().at(i)) + unitCounter,
                                    std::get<2>(form.getMinimums()[i]) + unitCounter) );
}

void Formula::addBinaryOperation(const Formula& form, LogicalSymbol binSym)
{
    std::vector<BinaryOperation> *binOp;

    if ( binSym == Lor )
        binOp = &lDisjunctions;
    else if ( binSym == Equiv )
        binOp = &equivalences;
    else if ( binSym == Impl )
        binOp = &implications;
    else if ( binSym == Max )
        binOp = &maximums;
    else if ( binSym == Min )
        binOp = &minimums;
    else
        throw std::invalid_argument("Not a valid logic operation.");

    addUnits(form);

    binOp->push_back( BinaryOperation(unitCounter + form.getUnitCounter() + 1,
                                      unitCounter,
                                      unitCounter + form.getUnitCounter()) );

    unitCounter += form.getUnitCounter() + 1;
}

void Formula::addLukaDisjunction(const Formula& form)
{
    if ( emptyFormula )
    {
        *this = form;
        emptyFormula = false;
    }
    else if ( ( unitCounter == 1 ) && ( form.unitCounter == 1 ) )
    {
        unitClauses.at(0).second.insert( unitClauses.at(0).second.end(),
                                         form.unitClauses.at(0).second.begin(),
                                         form.unitClauses.at(0).second.end() );
    }
    else
        addBinaryOperation(form,Lor);
}

void Formula::addEquivalence(const Formula& form)
{
    if ( !emptyFormula && !form.isEmpty() )
        addBinaryOperation(form,Equiv);
}

void Formula::addImplication(const Formula& form)
{
    if ( !emptyFormula && !form.isEmpty() )
        addBinaryOperation(form,Impl);
}

void Formula::addMaximum(const Formula& form)
{
    if ( emptyFormula )
    {
        *this = form;
        emptyFormula = false;
    }
    else
        addBinaryOperation(form,Max);
}

void Formula::addMinimum(const Formula& form)
{
    if ( emptyFormula )
    {
        *this = form;
        emptyFormula = false;
    }
    else
        addBinaryOperation(form,Min);
}

pwl2limodsat::Variable Formula::shiftVariables(std::vector<pwl2limodsat::Variable> newInputs,
                                               pwl2limodsat::Variable byVar)
{
    pwl2limodsat::Variable maximum = 0;

    for ( size_t i = 0; i < unitClauses.size(); i++ )
    {
        for ( size_t j = 0; j < unitClauses.at(i).second.size(); j++ )
        {
            pwl2limodsat::Variable newVariable;
            bool negVar = false;
            bool inputVar = false;

            if ( unitClauses.at(i).second.at(j) > 0 )
            {
                if ( (size_t) unitClauses.at(i).second.at(j) <= newInputs.size() )
                {
                    newVariable = newInputs.at( unitClauses.at(i).second.at(j) - 1 );
                    inputVar = true;
                }
                else
                    newVariable = (pwl2limodsat::Variable) unitClauses.at(i).second.at(j) + byVar;
            }
            else
            {
                if ( (size_t) -1*unitClauses.at(i).second.at(j) <= newInputs.size() )
                {
                    newVariable = newInputs.at( unitClauses.at(i).second.at(j) - 1 );
                    inputVar = true;
                }
                else
                    newVariable = (pwl2limodsat::Variable) std::abs(unitClauses.at(i).second.at(j)) + byVar;

                negVar = true;
            }

            if ( newVariable > maximum && !inputVar )
                maximum = newVariable;

            unitClauses.at(i).second.at(j) = (Literal) newVariable;

            if ( negVar )
                unitClauses.at(i).second.at(j) *= -1;
        }
    }

    return maximum;
}

void Formula::print(std::ofstream *output)
{
    unsigned unitClausesCounter = 0;
    unsigned negationsCounter = 0;
    unsigned equivalencesCounter = 0;
    unsigned implicationsCounter = 0;
    unsigned maximumsCounter = 0;
    unsigned minimumsCounter = 0;

    for ( unsigned i = 1; i <= unitCounter; i++ )
    {
        if ( (unitClausesCounter < unitClauses.size()) && (unitClauses.at(unitClausesCounter).first == i) )
        {
            *output << "Unit " << i << " :: Clause      :: ";
            for ( size_t j = 0; j < unitClauses.at(unitClausesCounter).second.size(); j++ )
                *output << unitClauses.at(unitClausesCounter).second.at(j) << " ";
            *output << std::endl;

            unitClausesCounter++;
        }
        else if ( (negationsCounter < negations.size()) && (negations.at(negationsCounter).first == i) )
        {
            *output << "Unit " << i << " :: Negation    :: ";
            *output << negations.at(negationsCounter).second << std::endl;

            negationsCounter++;
        }
        else if ( (equivalencesCounter < equivalences.size()) && (std::get<0>(equivalences.at(equivalencesCounter)) == i) )
        {
            *output << "Unit " << i << " :: Equivalence :: ";
            *output << std::get<1>(equivalences.at(equivalencesCounter))
                    << " "
                    << std::get<2>(equivalences.at(equivalencesCounter))
                    << std::endl;

            equivalencesCounter++;
        }
        else if ( (implicationsCounter < implications.size()) && (std::get<0>(implications.at(implicationsCounter)) == i) )
        {
            *output << "Unit " << i << " :: Implication :: ";
            *output << std::get<1>(implications.at(implicationsCounter))
                    << " "
                    << std::get<2>(implications.at(implicationsCounter))
                    << std::endl;

            implicationsCounter++;
        }
        else if ( (maximumsCounter < maximums.size()) && (std::get<0>(maximums.at(maximumsCounter)) == i) )
        {
            *output << "Unit " << i << " :: Maximum     :: ";
            *output << std::get<1>(maximums.at(maximumsCounter))
                    << " "
                    << std::get<2>(maximums.at(maximumsCounter))
                    << std::endl;

            maximumsCounter++;
        }
        else if ( (minimumsCounter < minimums.size()) && (std::get<0>(minimums.at(minimumsCounter)) == i) )
        {
            *output << "Unit " << i << " :: Minimum     :: ";
            *output << std::get<1>(minimums.at(minimumsCounter))
                    << " "
                    << std::get<2>(minimums.at(minimumsCounter))
                    << std::endl;

            minimumsCounter++;
        }
    }
}
}
