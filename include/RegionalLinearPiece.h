#ifndef REGIONALLINEARPIECE_H
#define REGIONALLINEARPIECE_H

#include "LinearPiece.h"

enum BoundarySymbol { GeqZero, LeqZero };

typedef pair<unsigned,BoundarySymbol> Boundary;

class RegionalLinearPiece : public LinearPiece
{
    public:
        RegionalLinearPiece(const vector<LinearPieceCoefficient>& coefs, const vector<Boundary>& bounds, VariableManager *varMan);

        bool isAbove(const RegionalLinearPiece& comparedRLP);

    protected:

    private:
        vector<Boundary> boundaries;
};

#endif // REGIONALLINEARPIECE_H
