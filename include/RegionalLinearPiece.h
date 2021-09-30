#ifndef REGIONALLINEARPIECE_H
#define REGIONALLINEARPIECE_H

#include "LinearPiece.h"

namespace pwl2limodsat
{
class RegionalLinearPiece : public LinearPiece
{
    public:
        RegionalLinearPiece(const RegionalLinearPieceData& rlpData,
                            const BoundaryPrototypeCollection *bpData,
                            VariableManager *varMan);
        bool comparedIsAbove(const RegionalLinearPiece& comparedRlp);
        bool comparedIsBelow(const RegionalLinearPiece& comparedRlp);
        LinearPieceData getLinearPieceData() const { return linearPieceData; }

    protected:

    private:
        BoundaryCollection boundaryData;
        const BoundaryPrototypeCollection *boundaryPrototypeData;
        enum Position { ComparedIsAbove, ComparedIsBelow };

        bool position(Position pos, const RegionalLinearPiece& comparedRlp);
};
}

#endif // REGIONALLINEARPIECE_H
