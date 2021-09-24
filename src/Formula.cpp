#include <stdexcept>

#include "Formula.h"

Formula::Formula() {}

Formula::Formula(const Clause& clau)
{
    unitClauses.push_back(UnitClause(++unitCounter, clau));
}

Formula::Formula(Literal lit)
{
    unitClauses.push_back(UnitClause(++unitCounter, Clause(1,lit)));
}

Formula::Formula(Variable var) :
    Formula((Literal) var) {}

Formula::Formula(const Formula& form, LogicalSymbol unSym) :
    Formula(form)
{
    if ( unSym == Neg )
        this->negateFormula();
    else
        throw invalid_argument("Not a valid logic operation.");
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
        throw invalid_argument("Not a valid logic operation.");
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
        lDisjunctions.push_back(LDisjunction(get<0>(form.getLDisjunctions().at(i)) + unitCounter,
                                get<1>(form.getLDisjunctions().at(i)) + unitCounter, get<2>(form.getLDisjunctions()[i]) + unitCounter));

    for ( size_t i = 0; i < form.getEquivalences().size(); i++ )
        equivalences.push_back(Equivalence(get<0>(form.getEquivalences().at(i)) + unitCounter,
                               get<1>(form.getEquivalences().at(i)) + unitCounter, get<2>(form.getEquivalences()[i]) + unitCounter));

    for ( size_t i = 0; i < form.getImplications().size(); i++ )
        implications.push_back(Implication(get<0>(form.getImplications().at(i)) + unitCounter,
                               get<1>(form.getImplications().at(i)) + unitCounter, get<2>(form.getImplications()[i]) + unitCounter));

    for ( size_t i = 0; i < form.getMaximums().size(); i++ )
        maximums.push_back(Maximum(get<0>(form.getMaximums().at(i)) + unitCounter,
                               get<1>(form.getMaximums().at(i)) + unitCounter, get<2>(form.getMaximums()[i]) + unitCounter));

    for ( size_t i = 0; i < form.getMinimums().size(); i++ )
        minimums.push_back(Minimum(get<0>(form.getMinimums().at(i)) + unitCounter,
                               get<1>(form.getMinimums().at(i)) + unitCounter, get<2>(form.getMinimums()[i]) + unitCounter));
}

void Formula::addBinaryOperation(const Formula& form, LogicalSymbol binSym)
{
    vector<BinaryOperation> *binOp;

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
        throw invalid_argument("Not a valid logic operation.");

    addUnits(form);

    binOp->push_back(BinaryOperation(unitCounter + form.getUnitCounter() + 1, unitCounter,
    unitCounter + form.getUnitCounter()));

    unitCounter += form.getUnitCounter() + 1;
}

void Formula::addLukaDisjunction(const Formula& form)
{
    if ( ( unitCounter <= 1 ) && ( form.unitCounter <= 1 ) )
    {
        if ( ( unitCounter == 1 ) && ( form.unitCounter == 1 ) )
            unitClauses.at(0).second.insert(unitClauses.at(0).second.end(),
            form.unitClauses.at(0).second.begin(), form.unitClauses.at(0).second.end());
        else if ( ( unitCounter == 0 ) && ( form.unitCounter == 1 ) )
            *this = form;
    }
    else
        addBinaryOperation(form,Lor);
}

void Formula::addEquivalence(const Formula& form)
{
    addBinaryOperation(form,Equiv);
}

void Formula::addImplication(const Formula& form)
{
    addBinaryOperation(form,Impl);
}

void Formula::addMaximum(const Formula& form)
{
    addBinaryOperation(form,Max);
}

void Formula::addMinimum(const Formula& form)
{
    addBinaryOperation(form,Min);
}

void Formula::print(ofstream *output)
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
            *output << endl;

            unitClausesCounter++;
        }
        else if ( (negationsCounter < negations.size()) && (negations.at(negationsCounter).first == i) )
        {
            *output << "Unit " << i << " :: Negation    :: ";
            *output << negations.at(negationsCounter).second << endl;

            negationsCounter++;
        }
        else if ( (equivalencesCounter < equivalences.size()) && (get<0>(equivalences.at(equivalencesCounter)) == i) )
        {
            *output << "Unit " << i << " :: Equivalence :: ";
            *output << get<1>(equivalences.at(equivalencesCounter)) << " " << get<2>(equivalences.at(equivalencesCounter)) << endl;

            equivalencesCounter++;
        }
        else if ( (implicationsCounter < implications.size()) && (get<0>(implications.at(implicationsCounter)) == i) )
        {
            *output << "Unit " << i << " :: Implication :: ";
            *output << get<1>(implications.at(implicationsCounter)) << " " << get<2>(implications.at(implicationsCounter)) << endl;

            implicationsCounter++;
        }
        else if ( (maximumsCounter < maximums.size()) && (get<0>(maximums.at(maximumsCounter)) == i) )
        {
            *output << "Unit " << i << " :: Maximum     :: ";
            *output << get<1>(maximums.at(maximumsCounter)) << " " << get<2>(maximums.at(maximumsCounter)) << endl;

            maximumsCounter++;
        }
        else if ( (minimumsCounter < minimums.size()) && (get<0>(minimums.at(minimumsCounter)) == i) )
        {
            *output << "Unit " << i << " :: Minimum     :: ";
            *output << get<1>(minimums.at(minimumsCounter)) << " " << get<2>(minimums.at(minimumsCounter)) << endl;

            minimumsCounter++;
        }
    }
}
