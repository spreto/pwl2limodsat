#ifndef PIECEWISELINEARFUNCTION_H
#define PIECEWISELINEARFUNCTION_H

#include "RegionalLinearPiece.h"

enum ProcessingMode { Single, Multi };

typedef float BoundaryCoefficient;
typedef vector<BoundaryCoefficient> BoundaryPrototype;

class PiecewiseLinearFunction
{
    public:
        PiecewiseLinearFunction(const vector<vector<LinearPieceCoefficient>>& coefss,
                                const vector<vector<Boundary>>& boundss,
                                const vector<BoundaryPrototype>& boundProts,
                                string inputFileName,
                                bool multithreading);
        PiecewiseLinearFunction(const vector<vector<LinearPieceCoefficient>>& coefss,
                                const vector<vector<Boundary>>& boundss,
                                const vector<BoundaryPrototype>& boundProts,
                                string inputFileName);
        void setProcessingMode(ProcessingMode mode) { processingMode = mode; }
        void representModSat();
        void printRepresentation();

    protected:

    private:
        string outputFileName;
        ProcessingMode processingMode;

        vector<RegionalLinearPiece> pieces;
        vector<BoundaryPrototype> boundaryPrototypes;
        Formula latticeFormula;

        VariableManager var;
        bool modsatTranslation = false;

        void representPiecesModSat();
        vector<Formula> partialPhiOmega(unsigned thread, unsigned compByThread);
        void representLatticeFormula(unsigned maxThreadsNum);
};

#endif // PIECEWISELINEARFUNCTION_H
