#ifndef REGIONALLINEARPIECE_H
#define REGIONALLINEARPIECE_H

#include "LinearPiece.h"

enum BoundarySymbol { GeqZero, LeqZero };

typedef pair<unsigned,BoundarySymbol> Boundary;
typedef float BoundaryCoefficient;
typedef vector<BoundaryCoefficient> BoundaryPrototype;

enum Position { Above, Below };

class RegionalLinearPiece : public LinearPiece
{
    public:
        RegionalLinearPiece(const vector<LinearPieceCoefficient>& coefs,
                            const vector<Boundary>& bounds,
                            const vector<BoundaryPrototype> *boundProts,
                            VariableManager *varMan);
        bool isAbove(const RegionalLinearPiece& comparedRLP);
        bool isBelow(const RegionalLinearPiece& comparedRLP);
        vector<LinearPieceCoefficient> getPieceCoefficients() const { return pieceCoefficients; }

    protected:

    private:
        vector<Boundary> boundaries;
        const vector<BoundaryPrototype> *boundaryPrototypes;

        bool position(Position pos, const RegionalLinearPiece& comparedRLP);
};

#endif // REGIONALLINEARPIECE_H
