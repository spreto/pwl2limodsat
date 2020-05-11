#ifndef FORMULA_H
#define FORMULA_H

#include <vector>
#include <tuple>
#include <fstream>

using namespace std;

enum LogicalSymbol { Neg, Lor, Equiv, Impl, Max, Min };

typedef int Literal;
typedef vector<Literal> Clause;
typedef pair<unsigned,Clause> UnitClause;
typedef pair<unsigned,unsigned> Negation;
typedef tuple<unsigned,unsigned,unsigned> BinaryOperation;
typedef tuple<unsigned,unsigned,unsigned> LDisjunction;
typedef tuple<unsigned,unsigned,unsigned> Equivalence;
typedef tuple<unsigned,unsigned,unsigned> Implication;
typedef tuple<unsigned,unsigned,unsigned> Maximum;
typedef tuple<unsigned,unsigned,unsigned> Minimum;

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
        vector<UnitClause> getUnitClauses() const { return unitClauses; }
        vector<Negation> getNegations() const { return negations; }
        vector<LDisjunction> getLDisjunctions() const { return lDisjunctions; }
        vector<Equivalence> getEquivalences() const { return equivalences; }
        vector<Implication> getImplications() const { return implications; }
        vector<Maximum> getMaximums() const { return maximums; }
        vector<Minimum> getMinimums() const { return minimums; }

        void print(ofstream *output);

    private:
        unsigned unitCounter = 0;
        vector<UnitClause> unitClauses;
        vector<Negation> negations;
        vector<LDisjunction> lDisjunctions;
        vector<Equivalence> equivalences;
        vector<Implication> implications;
        vector<Maximum> maximums;
        vector<Minimum> minimums;

        void addUnits(const Formula& form);
        void addBinaryOperation(const Formula& form, LogicalSymbol binSym);
};

typedef vector<Formula> ModSatSet;

struct ModSat
{
    Formula phi;
    ModSatSet Phi;
};

#endif // FORMULA_H
