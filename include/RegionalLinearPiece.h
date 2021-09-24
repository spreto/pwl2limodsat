#ifndef REGIONALLINEARPIECE_H
#define REGIONALLINEARPIECE_H

#include "LinearPiece.h"

enum BoundarySymbol { GeqZero, LeqZero };

typedef pair<unsigned,BoundarySymbol> Boundary;
typedef float BoundaryCoefficient;
typedef vector<BoundaryCoefficient> BoundaryPrototype;

class RegionalLinearPiece : public LinearPiece
{
    public:
        RegionalLinearPiece(const vector<LinearPieceCoefficient>& coefs,
                            const vector<Boundary>& bounds,
                            const vector<BoundaryPrototype> *boundProts,
                            VariableManager *varMan);

        vector<LinearPieceCoefficient> getPieceCoefficients() const { return pieceCoefficients; }

        float precision(float num);
        bool isAbove(const RegionalLinearPiece& comparedRLP);

    protected:

    private:
        vector<Boundary> boundaries;
        const vector<BoundaryPrototype> *boundaryPrototypes;
};

#endif // REGIONALLINEARPIECE_H
