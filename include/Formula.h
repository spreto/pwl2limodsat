#ifndef FORMULA_H
#define FORMULA_H

#include <vector>
#include <tuple>
#include <fstream>
#include "pwl2limodsat.h"

namespace lukaFormula
{
enum LogicalSymbol { Neg, Lor, Land, Equiv, Impl, Max, Min };

typedef int Literal;
typedef unsigned UnitIndex;

typedef std::vector<Literal> Clause;
typedef std::pair<UnitIndex,Clause> UnitClause;
typedef std::pair<UnitIndex,UnitIndex> Negation;
typedef std::tuple<UnitIndex,UnitIndex,UnitIndex> BinaryOperation;
typedef std::tuple<UnitIndex,UnitIndex,UnitIndex> LDisjunction;
typedef std::tuple<UnitIndex,UnitIndex,UnitIndex> LConjunction;
typedef std::tuple<UnitIndex,UnitIndex,UnitIndex> Equivalence;
typedef std::tuple<UnitIndex,UnitIndex,UnitIndex> Implication;
typedef std::tuple<UnitIndex,UnitIndex,UnitIndex> Maximum;
typedef std::tuple<UnitIndex,UnitIndex,UnitIndex> Minimum;

class Formula
{
    public:
        Formula();
        Formula(const Clause& clau);
        Formula(Literal lit);
        Formula(pwl2limodsat::Variable var);
        Formula(const Formula& form, LogicalSymbol unSym);
        Formula(const Formula& form1, const Formula& form2, LogicalSymbol binSym);
        Formula(std::vector<UnitClause> unitClausesInput,
                std::vector<Negation> negationsInput,
                std::vector<LDisjunction> lDisjunctionsInput,
                std::vector<LConjunction> lConjunctionsInput,
                std::vector<Equivalence> equivalencesInput,
                std::vector<Implication> implicationsInput,
                std::vector<Maximum> maximumsInput,
                std::vector<Minimum> minimumsInput);

        bool isEmpty() const { return emptyFormula; }

        void negateFormula();
        void addLukaDisjunction(const Formula& form);
        void addLukaConjunction(const Formula& form);
        void addEquivalence(const Formula& form);
        void addImplication(const Formula& form);
        void addMaximum(const Formula& form);
        void addMinimum(const Formula& form);
        pwl2limodsat::Variable shiftVariables(std::vector<pwl2limodsat::Variable> newInputs,
                                              pwl2limodsat::Variable byVar);
        unsigned getUnitCounter() const { return unitCounter; }
        std::vector<UnitClause> getUnitClauses() const { return unitClauses; }
        std::vector<Negation> getNegations() const { return negations; }
        std::vector<LDisjunction> getLDisjunctions() const { return lDisjunctions; }
        std::vector<LDisjunction> getLConjunctions() const { return lConjunctions; }
        std::vector<Equivalence> getEquivalences() const { return equivalences; }
        std::vector<Implication> getImplications() const { return implications; }
        std::vector<Maximum> getMaximums() const { return maximums; }
        std::vector<Minimum> getMinimums() const { return minimums; }

        void print(std::ofstream *output);

    private:
        bool emptyFormula = false;
        UnitIndex unitCounter = 0;
        std::vector<UnitClause> unitClauses;
        std::vector<Negation> negations;
        std::vector<LDisjunction> lDisjunctions;
        std::vector<LConjunction> lConjunctions;
        std::vector<Equivalence> equivalences;
        std::vector<Implication> implications;
        std::vector<Maximum> maximums;
        std::vector<Minimum> minimums;

        void addUnits(const Formula& form);
        void addBinaryOperation(const Formula& form, LogicalSymbol binSym);
};

typedef std::vector<Formula> ModsatSet;

struct Modsat
{
    Formula phi;
    ModsatSet Phi;
};
}

#endif // FORMULA_H
