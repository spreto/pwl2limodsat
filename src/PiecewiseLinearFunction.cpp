#include "PiecewiseLinearFunction.h"

using namespace std;

PiecewiseLinearFunction::PiecewiseLinearFunction(const vector<vector<LinearPieceCoefficient>>& coefss, const vector<vector<Boundary>>& boundss,
                                                 const vector<BoundaryPrototype>& boundProts, string inputFileName) :
    boundaryPrototypes(boundProts),
    var(VariableManager(coefss.at(0).size()-1))
{
    for ( auto i = 0; i < coefss.size(); i++ )
        pieces.push_back(RegionalLinearPiece(coefss.at(i), boundss.at(i), &boundaryPrototypes, &var));

    if ( inputFileName.substr(inputFileName.size()-4,4) == ".pwl" )
        outputFileName = inputFileName.substr(0,inputFileName.size()-4);
    else
        outputFileName = inputFileName;

    outputFileName.append(".out");
}

void PiecewiseLinearFunction::representPiecesModSat()
{
    for ( auto i = 0; i < pieces.size(); i++ )
        pieces.at(i).representModSat();
}

void PiecewiseLinearFunction::representLatticeFormula()
{
    representPiecesModSat();

    vector<Formula> phiOmega;

    for ( auto i = 0; i < pieces.size(); i++ )
        phiOmega.push_back(pieces.at(i).getRepresentationModSat().phi);

    for ( auto i = 0; i < pieces.size(); i++ )
        for ( auto k = 0; k < pieces.size(); k++ )
            if ( i != k )
                if ( pieces.at(i).isAbove(pieces.at(k)) )
                    phiOmega.at(i).addMinimum(pieces.at(k).getRepresentationModSat().phi);

    latticeFormula = phiOmega.at(0);
    for ( auto i = 1; i < pieces.size(); i++ )
        latticeFormula.addMaximum(phiOmega.at(i));
}

void PiecewiseLinearFunction::printRepresentation()
{
    ofstream outputFile(outputFileName);

    outputFile << "-= Formula phi =-" << endl << endl;
    latticeFormula.print(&outputFile);

    outputFile << endl << "-= MODSAT Set Phi =-" << endl;

    for ( auto i = 0; i < pieces.size(); i++ )
    {
        outputFile << endl << "-= Linear Piece " << i+1 << " =-" << endl;
        pieces.at(i).printModSatSet(&outputFile);
    }
}
