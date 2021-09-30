#ifndef PIECEWISELINEARFUNCTION_H
#define PIECEWISELINEARFUNCTION_H

#include "RegionalLinearPiece.h"

namespace pwl2limodsat
{
class PiecewiseLinearFunction
{
    public:
        PiecewiseLinearFunction(const PiecewiseLinearFunctionData& pwlData,
                                const BoundaryPrototypeCollection& boundProtData,
                                std::string inputFileName,
                                bool multithreading);
        PiecewiseLinearFunction(const PiecewiseLinearFunctionData& pwlData,
                                const BoundaryPrototypeCollection& boundProtData,
                                std::string inputFileName);
        bool hasLatticeProperty();
        void representModSat();
        void printRepresentation();

    protected:

    private:
        std::string outputFileName;
        enum ProcessingMode { Single, Multi };
        ProcessingMode processingMode;

        std::vector<RegionalLinearPiece> linearPieceCollection;
        BoundaryPrototypeCollection boundaryPrototypeData;
        Formula latticeFormula;

        VariableManager var;
        bool modsatTranslation = false;

        void setProcessingMode(ProcessingMode mode) { processingMode = mode; }
        void representPiecesModSat();
        std::vector<Formula> partialPhiOmega(unsigned thread, unsigned compByThread);
        void representLatticeFormula(unsigned maxThreadsNum);
};
}

#endif // PIECEWISELINEARFUNCTION_H
