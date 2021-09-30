#ifndef PWL2LIMODSAT_H_INCLUDED
#define PWL2LIMODSAT_H_INCLUDED

#include <vector>

namespace pwl2limodsat
{
enum ExecutionMode { PWL, TL };

typedef int LPCoefInteger;
typedef unsigned LPCoefNonNegative;
typedef std::pair<LPCoefInteger,LPCoefNonNegative> LinearPieceCoefficient;
typedef std::vector<LinearPieceCoefficient> LinearPieceData;

typedef size_t BoundProtIndex;
enum BoundarySymbol { GeqZero, LeqZero };
typedef std::pair<BoundProtIndex,BoundarySymbol> Boundary;
typedef std::vector<Boundary> BoundaryCollection;

struct RegionalLinearPieceData
{
    LinearPieceData lpData;
    BoundaryCollection bound;
};
typedef std::vector<RegionalLinearPieceData> PiecewiseLinearFunctionData;

typedef double BoundaryCoefficient;
typedef std::vector<BoundaryCoefficient> BoundaryPrototype;
typedef std::vector<BoundaryPrototype> BoundaryPrototypeCollection;

typedef unsigned Variable;
}

#endif // PWL2LIMODSAT_H_INCLUDED
