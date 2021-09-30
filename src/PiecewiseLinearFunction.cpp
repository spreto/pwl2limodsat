#include "PiecewiseLinearFunction.h"
#include <iostream>
#include <future>
#include <cmath>

namespace pwl2limodsat
{
PiecewiseLinearFunction::PiecewiseLinearFunction(const PiecewiseLinearFunctionData& pwlData,
                                                 const BoundaryPrototypeCollection& boundProtData,
                                                 std::string inputFileName,
                                                 bool multithreading) :
    boundaryPrototypeData(boundProtData),
    var(VariableManager(pwlData.at(0).lpData.size() - 1))
{
    for ( size_t i = 0; i < pwlData.size(); i++ )
        linearPieceCollection.push_back(RegionalLinearPiece(pwlData.at(i), &boundaryPrototypeData, &var));

    if ( inputFileName.substr(inputFileName.size()-4,4) == ".pwl" )
        outputFileName = inputFileName.substr(0,inputFileName.size()-4);
    else
        outputFileName = inputFileName;

    processingMode = ( multithreading ? Multi : Single );

    outputFileName.append(".limodsat");
}

PiecewiseLinearFunction::PiecewiseLinearFunction(const PiecewiseLinearFunctionData& pwlData,
                                                 const BoundaryPrototypeCollection& boundProtData,
                                                 std::string inputFileName) :
    PiecewiseLinearFunction(pwlData, boundProtData, inputFileName, false) {}

bool PiecewiseLinearFunction::hasLatticeProperty()
{
    bool hasLatticeProperty = true;

    for ( size_t i = 0; i < linearPieceCollection.size() && hasLatticeProperty; i++ )
        for ( size_t j = 0; j < linearPieceCollection.size() && hasLatticeProperty; j++ )
            if ( i != j )
            {
                bool found = false;

                for ( size_t k = 0; k < linearPieceCollection.size() && !found; k++ )
                {
                    if ( linearPieceCollection.at(i).comparedIsBelow(linearPieceCollection.at(k)) )
                        if ( linearPieceCollection.at(j).comparedIsAbove(linearPieceCollection.at(k)) )
                            found = true;
                }

                if ( !found )
                    hasLatticeProperty = false;
            }

    return hasLatticeProperty;
}

void PiecewiseLinearFunction::representPiecesModSat()
{
    for ( size_t i = 0; i < linearPieceCollection.size(); i++ )
        linearPieceCollection.at(i).representModSat();
}

std::vector<Formula> PiecewiseLinearFunction::partialPhiOmega(unsigned thread, unsigned compByThread)
{
    std::vector<Formula> partPhiOmega;

    for ( size_t i = thread * compByThread; i < (thread + 1) * compByThread; i++ )
    {
        partPhiOmega.push_back( linearPieceCollection.at(i).getRepresentationModSat().phi );

        for ( size_t k = 0; k < linearPieceCollection.size(); k++ )
            if ( k != i )
                if ( linearPieceCollection.at(i).comparedIsAbove(linearPieceCollection.at(k)) )
                    partPhiOmega.back().addMinimum(linearPieceCollection.at(k).getRepresentationModSat().phi);
    }

    return partPhiOmega;
}

void PiecewiseLinearFunction::representLatticeFormula(unsigned maxThreadsNum)
{
    unsigned compByThread = ceil( (float) linearPieceCollection.size() / (float) maxThreadsNum );
    unsigned threadsNum = ceil( (float) linearPieceCollection.size() / (float) compByThread );

    std::vector<std::future<std::vector<Formula>>> phiOmegaFut;
    for ( unsigned thread = 0; thread < threadsNum - 1; thread++ )
        phiOmegaFut.push_back( async(&PiecewiseLinearFunction::partialPhiOmega, this, thread, compByThread) );

    std::vector<Formula> phiOmegaFirst;
    for ( size_t i = (threadsNum - 1) * compByThread; i < linearPieceCollection.size(); i++ )
    {
        phiOmegaFirst.push_back( linearPieceCollection.at(i).getRepresentationModSat().phi );

        for ( size_t k = 0; k < linearPieceCollection.size(); k++ )
            if ( k != i )
                if ( linearPieceCollection.at(i).comparedIsAbove(linearPieceCollection.at(k)) )
                    phiOmegaFirst.back().addMinimum(linearPieceCollection.at(k).getRepresentationModSat().phi);
    }

    latticeFormula = phiOmegaFirst.at(0);
    for ( size_t i = 1; i < phiOmegaFirst.size(); i++ )
        latticeFormula.addMaximum(phiOmegaFirst.at(i));

    for ( unsigned thread = 0; thread < threadsNum - 1; thread++ )
    {
        std::vector<Formula> phiOmega = phiOmegaFut.at(thread).get();

        for ( size_t i = 0; i < phiOmega.size(); i++ )
            latticeFormula.addMaximum(phiOmega.at(i));
    }
}

void PiecewiseLinearFunction::representModSat()
{
    representPiecesModSat();

    if ( processingMode == Multi )
        representLatticeFormula(std::thread::hardware_concurrency());
    else if ( processingMode == Single )
        representLatticeFormula(1);

    modsatTranslation = true;
}

void PiecewiseLinearFunction::printRepresentation()
{
    if ( !modsatTranslation )
        representModSat();

    std::ofstream outputFile(outputFileName);

    outputFile << "-= Formula phi =-" << std::endl << std::endl;
    latticeFormula.print(&outputFile);

    outputFile << std::endl << "-= MODSAT Set Phi =-" << std::endl;

    for ( size_t i = 0; i < linearPieceCollection.size(); i++ )
    {
        outputFile << std::endl << "-= Linear Piece " << i+1 << " =-" << std::endl;
        linearPieceCollection.at(i).printModSatSet(&outputFile);
    }
}
}
