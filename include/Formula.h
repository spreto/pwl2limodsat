#ifndef FORMULA_H
#define FORMULA_H

#include <vector>
#include <tuple>
#include <fstream>
#include "pwl2limodsat.h"

namespace lukaFormula
{
enum LogicalSymbol { Neg, Lor, Equiv, Impl, Max, Min };

typedef int Literal;
typedef unsigned UnitClauseIndex;

typedef std::vector<Literal> Clause;
typedef std::pair<UnitClauseIndex,Clause> UnitClause;
typedef std::pair<UnitClauseIndex,UnitClauseIndex> Negation;
typedef std::tuple<UnitClauseIndex,UnitClauseIndex,UnitClauseIndex> BinaryOperation;
typedef std::tuple<UnitClauseIndex,UnitClauseIndex,UnitClauseIndex> LDisjunction;
typedef std::tuple<UnitClauseIndex,UnitClauseIndex,UnitClauseIndex> Equivalence;
typedef std::tuple<UnitClauseIndex,UnitClauseIndex,UnitClauseIndex> Implication;
typedef std::tuple<UnitClauseIndex,UnitClauseIndex,UnitClauseIndex> Maximum;
typedef std::tuple<UnitClauseIndex,UnitClauseIndex,UnitClauseIndex> Minimum;

class Formula
{
    public:
        Formula();
        Formula(const Clause& clau);
        Formula(Literal lit);
        Formula(pwl2limodsat::Variable var);
        Formula(const Formula& form, LogicalSymbol unSym);
        Formula(const Formula& form1, const Formula& form2, LogicalSymbol binSym);

        bool isEmpty() const { return emptyFormula; }

        void negateFormula();
        void addLukaDisjunction(const Formula& form);
        void addEquivalence(const Formula& form);
        void addImplication(const Formula& form);
        void addMaximum(const Formula& form);
        void addMinimum(const Formula& form);

        unsigned getUnitCounter() const { return unitCounter; }
        std::vector<UnitClause> getUnitClauses() const { return unitClauses; }
        std::vector<Negation> getNegations() const { return negations; }
        std::vector<LDisjunction> getLDisjunctions() const { return lDisjunctions; }
        std::vector<Equivalence> getEquivalences() const { return equivalences; }
        std::vector<Implication> getImplications() const { return implications; }
        std::vector<Maximum> getMaximums() const { return maximums; }
        std::vector<Minimum> getMinimums() const { return minimums; }

        void print(std::ofstream *output);

    private:
        bool emptyFormula = false;
        unsigned unitCounter = 0;
        std::vector<UnitClause> unitClauses;
        std::vector<Negation> negations;
        std::vector<LDisjunction> lDisjunctions;
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
