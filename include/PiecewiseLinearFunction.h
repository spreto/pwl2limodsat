#ifndef PIECEWISELINEARFUNCTION_H
#define PIECEWISELINEARFUNCTION_H

#include "RegionalLinearPiece.h"

typedef float BoundaryCoefficient;
typedef vector<BoundaryCoefficient> BoundaryPrototype;

class PiecewiseLinearFunction
{
    public:
        PiecewiseLinearFunction(const vector<vector<LinearPieceCoefficient>>& coefss, const vector<vector<Boundary>>& boundss,
                                const vector<BoundaryPrototype>& boundProts, string inputFileName);

        void representLatticeFormula();
        void printRepresentation();

    protected:

    private:
        string outputFileName;
        vector<RegionalLinearPiece> pieces;
        vector<BoundaryPrototype> boundaryPrototypes;
        Formula latticeFormula;
        VariableManager var;

        void representPiecesModSat();
};

#endif // PIECEWISELINEARFUNCTION_H
