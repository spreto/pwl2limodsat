#ifndef FORMULA_H
#define FORMULA_H

#include <vector>
#include <tuple>
#include <fstream>

enum LogicalSymbol { Neg, Lor, Equiv, Impl, Max, Min };

typedef int Literal;
typedef std::vector<Literal> Clause;
typedef std::pair<unsigned,Clause> UnitClause;
typedef std::pair<unsigned,unsigned> Negation;
typedef std::tuple<unsigned,unsigned,unsigned> BinaryOperation;
typedef std::tuple<unsigned,unsigned,unsigned> LDisjunction;
typedef std::tuple<unsigned,unsigned,unsigned> Equivalence;
typedef std::tuple<unsigned,unsigned,unsigned> Implication;
typedef std::tuple<unsigned,unsigned,unsigned> Maximum;
typedef std::tuple<unsigned,unsigned,unsigned> Minimum;

typedef unsigned Variable;

class Formula
{
    public:
        Formula();
        Formula(const Clause& clau);
        Formula(Literal lit);
        Formula(Variable var);

        Formula(const Formula& form, LogicalSymbol unSym);
        Formula(const Formula& form1, const Formula& form2, LogicalSymbol binSym);

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

typedef std::vector<Formula> ModSatSet;

struct ModSat
{
    Formula phi;
    ModSatSet Phi;
};

#endif // FORMULA_H
