#include "RegionalLinearPiece.h"

#include "soplex.h"

#define PREC 100000

namespace pwl2limodsat
{
RegionalLinearPiece::RegionalLinearPiece(const RegionalLinearPieceData& rlpData,
                                         const BoundaryPrototypeCollection *bpData,
                                         VariableManager *varMan) :
    LinearPiece(rlpData.lpData, varMan),
    boundaryData(rlpData.bound),
    boundaryPrototypeData(bpData) {}

bool RegionalLinearPiece::position(Position pos, const RegionalLinearPiece& comparedRlp)
{
    std::vector<float> objFunc;

    if ( pos == ComparedIsAbove )
        for ( unsigned i = 0; i <= dim; i++ )
            objFunc.push_back( ( (float) linearPieceData.at(i).first /
                                 (float) linearPieceData.at(i).second ) -
                               ( (float) comparedRlp.getLinearPieceData().at(i).first /
                                 (float) comparedRlp.getLinearPieceData().at(i).second ) );
    else if ( pos == ComparedIsBelow )
        for ( unsigned i = 0; i <= dim; i++ )
            objFunc.push_back( ( (float) comparedRlp.getLinearPieceData().at(i).first /
                                 (float) comparedRlp.getLinearPieceData().at(i).second ) -
                               ( (float) linearPieceData.at(i).first /
                                 (float) linearPieceData.at(i).second ) );

    float K = -objFunc.at(0);

    soplex::SoPlex sop;

    soplex::DSVector dummycol(0);
    for ( unsigned i = 1; i <= dim; i++ )
        sop.addColReal(soplex::LPCol(objFunc.at(i), dummycol, 1, 0));

    soplex::DSVector row(dim);
    for ( size_t i = 0; i < boundaryData.size(); i++ )
    {
        for ( size_t j = 1; j <= dim; j++ )
            row.add(j-1, boundaryPrototypeData->at(boundaryData.at(i).first).at(j));

        if ( boundaryData.at(i).second == GeqZero )
            sop.addRowReal(soplex::LPRow(-boundaryPrototypeData->at(boundaryData.at(i).first).at(0), row, soplex::infinity));
        else if ( boundaryData.at(i).second == LeqZero )
            sop.addRowReal(soplex::LPRow(-soplex::infinity, row, -boundaryPrototypeData->at(boundaryData.at(i).first).at(0)));

        row.clear();
    }

    sop.setIntParam(soplex::SoPlex::VERBOSITY, soplex::SoPlex::VERBOSITY_ERROR);
    sop.setIntParam(soplex::SoPlex::OBJSENSE, soplex::SoPlex::OBJSENSE_MAXIMIZE);
    sop.optimize();
    float Max = sop.objValueReal();

    if ( ( K > Max ) || ( abs(K-Max) < (float) 1/PREC ) )
        return true;
    else
        return false;
}

bool RegionalLinearPiece::comparedIsAbove(const RegionalLinearPiece& comparedRlp)
{
    return position(ComparedIsAbove, comparedRlp);
}

bool RegionalLinearPiece::comparedIsBelow(const RegionalLinearPiece& comparedRlp)
{
    return position(ComparedIsBelow, comparedRlp);
}
}
