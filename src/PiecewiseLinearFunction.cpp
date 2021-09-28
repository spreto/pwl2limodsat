#include "PiecewiseLinearFunction.h"
#include <iostream>
#include <future>
#include <math.h>

using namespace std;

PiecewiseLinearFunction::PiecewiseLinearFunction(const vector<vector<LinearPieceCoefficient>>& coefss,
                                                 const vector<vector<Boundary>>& boundss,
                                                 const vector<BoundaryPrototype>& boundProts,
                                                 string inputFileName,
                                                 bool multithreading) :
    boundaryPrototypes(boundProts),
    var(VariableManager(coefss.at(0).size()-1))
{
    for ( size_t i = 0; i < coefss.size(); i++ )
        pieces.push_back(RegionalLinearPiece(coefss.at(i), boundss.at(i), &boundaryPrototypes, &var));

    if ( inputFileName.substr(inputFileName.size()-4,4) == ".pwl" )
        outputFileName = inputFileName.substr(0,inputFileName.size()-4);
    else
        outputFileName = inputFileName;

    processingMode = ( multithreading ? Multi : Single );

    outputFileName.append(".out");
}

PiecewiseLinearFunction::PiecewiseLinearFunction(const vector<vector<LinearPieceCoefficient>>& coefss,
                                                 const vector<vector<Boundary>>& boundss,
                                                 const vector<BoundaryPrototype>& boundProts,
                                                 string inputFileName) :
    PiecewiseLinearFunction(coefss, boundss, boundProts, inputFileName, false) {}

void PiecewiseLinearFunction::representPiecesModSat()
{
    for ( size_t i = 0; i < pieces.size(); i++ )
        pieces.at(i).representModSat();
}

vector<Formula> PiecewiseLinearFunction::partialPhiOmega(unsigned thread, unsigned compByThread)
{
    vector<Formula> partPhiOmega;

    for ( size_t i = thread * compByThread; i < (thread + 1) * compByThread; i++ )
    {
        partPhiOmega.push_back( pieces.at(i).getRepresentationModSat().phi );

        for ( size_t k = 0; k < pieces.size(); k++ )
            if ( k != i )
                if ( pieces.at(i).isAbove(pieces.at(k)) )
                    partPhiOmega.back().addMinimum(pieces.at(k).getRepresentationModSat().phi);
    }

    return partPhiOmega;
}

void PiecewiseLinearFunction::representLatticeFormula(unsigned maxThreadsNum)
{
    unsigned compByThread = ceil( (float) pieces.size() / (float) maxThreadsNum );
    unsigned threadsNum = ceil( (float) pieces.size() / (float) compByThread );

    vector<future<vector<Formula>>> phiOmegaFut;
    for ( unsigned thread = 0; thread < threadsNum - 1; thread++ )
        phiOmegaFut.push_back( async(&PiecewiseLinearFunction::partialPhiOmega, this, thread, compByThread) );

    vector<Formula> phiOmegaFirst;
    for ( size_t i = (threadsNum - 1) * compByThread; i < pieces.size(); i++ )
    {
        phiOmegaFirst.push_back( pieces.at(i).getRepresentationModSat().phi );

        for ( size_t k = 0; k < pieces.size(); k++ )
            if ( k != i )
                if ( pieces.at(i).isAbove(pieces.at(k)) )
                    phiOmegaFirst.back().addMinimum(pieces.at(k).getRepresentationModSat().phi);
    }

    latticeFormula = phiOmegaFirst.at(0);
    for ( size_t i = 1; i < phiOmegaFirst.size(); i++ )
        latticeFormula.addMaximum(phiOmegaFirst.at(i));

    for ( unsigned thread = 0; thread < threadsNum - 1; thread++ )
    {
        vector<Formula> phiOmega = phiOmegaFut.at(thread).get();

        for ( size_t i = 0; i < phiOmega.size(); i++ )
            latticeFormula.addMaximum(phiOmega.at(i));
    }
}

void PiecewiseLinearFunction::representModSat()
{
    representPiecesModSat();

    if ( processingMode == Multi )
        representLatticeFormula(thread::hardware_concurrency());
    else if ( processingMode == Single )
        representLatticeFormula(1);

    modsatTranslation = true;
}

void PiecewiseLinearFunction::printRepresentation()
{
    if ( !modsatTranslation )
        representModSat();

    ofstream outputFile(outputFileName);

    outputFile << "-= Formula phi =-" << endl << endl;
    latticeFormula.print(&outputFile);

    outputFile << endl << "-= MODSAT Set Phi =-" << endl;

    for ( size_t i = 0; i < pieces.size(); i++ )
    {
        outputFile << endl << "-= Linear Piece " << i+1 << " =-" << endl;
        pieces.at(i).printModSatSet(&outputFile);
    }
}
