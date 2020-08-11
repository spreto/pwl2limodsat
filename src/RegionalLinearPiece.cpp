#include "RegionalLinearPiece.h"

#include "soplex.h"

#define PREC 100000

using namespace soplex;

RegionalLinearPiece::RegionalLinearPiece(const vector<LinearPieceCoefficient>& coefs, const vector<Boundary>& bounds,
                                         const vector<BoundaryPrototype> *boundProts, VariableManager *varMan) :
    LinearPiece(coefs, varMan),
    boundaries(bounds),
    boundaryPrototypes(boundProts) {}

bool RegionalLinearPiece::isAbove(const RegionalLinearPiece& comparedRLP)
{
    vector<float> objFunc;

    for ( auto i = 0; i <= dim; i++ )
        objFunc.push_back(((float) pieceCoefficients.at(i).first/(float) pieceCoefficients.at(i).second)-
                          ((float) comparedRLP.getPieceCoefficients().at(i).first/(float) comparedRLP.getPieceCoefficients().at(i).second));

    float K = -objFunc.at(0);

    SoPlex sop;

    DSVector dummycol(0);
    for ( auto i = 1; i <= dim; i++ )
        sop.addColReal(LPCol(objFunc.at(i), dummycol, 1, 0));

    DSVector row(dim);
    for ( auto i = 0; i < boundaries.size(); i++ )
    {
        for ( auto j = 1; j <= dim; j++ )
            row.add(j-1, boundaryPrototypes->at(boundaries.at(i).first).at(j));

        if ( boundaries.at(i).second == GeqZero )
            sop.addRowReal(LPRow(-boundaryPrototypes->at(boundaries.at(i).first).at(0), row, infinity));
        else if ( boundaries.at(i).second == LeqZero )
            sop.addRowReal(LPRow(-infinity, row, -boundaryPrototypes->at(boundaries.at(i).first).at(0)));

        row.clear();
    }

    sop.setIntParam(SoPlex::VERBOSITY, SoPlex::VERBOSITY_ERROR);
    sop.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MAXIMIZE);
    sop.optimize();
    float Max = sop.objValueReal();

    if ( ( K > Max ) || ( abs(K-Max) < (float) 1/PREC ) )
        return true;
    else
        return false;
}
