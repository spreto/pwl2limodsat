#include "RegionalLinearPiece.h"

RegionalLinearPiece::RegionalLinearPiece(const vector<LinearPieceCoefficient>& coefs, const vector<Boundary>& bounds, VariableManager *varMan) :
    LinearPiece(coefs, varMan),
    boundaries(bounds) {}

bool RegionalLinearPiece::isAbove(const RegionalLinearPiece& comparedRLP)
{
    return true;
}
