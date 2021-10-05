#include <cmath>
#include "LinearPiece.h"

namespace pwl2limodsat
{
LinearPiece::LinearPiece(const LinearPieceData& data, VariableManager *varMan) :
    linearPieceData(data),
    dim(linearPieceData.size()-1),
    var(varMan) {}

LinearPiece::LinearPiece(const LinearPieceData& data, std::string inputFileName) :
    linearPieceData(data),
    dim(linearPieceData.size()-1)
{
    var = new VariableManager(dim);
    ownVariableManager = true;

    if ( inputFileName.substr(inputFileName.size()-4,4) == ".pwl" )
        outputFileName = inputFileName.substr(0,inputFileName.size()-4);
    else
        outputFileName = inputFileName;

    outputFileName.append(".limodsat");
}

LinearPiece::~LinearPiece()
{
    if ( ownVariableManager )
        delete var;
}

Formula LinearPiece::zeroFormula()
{
    Clause zeroClau = { -(Literal) var->zeroVariable(), (Literal) var->zeroVariable() };
    return Formula( Formula(zeroClau), Neg );
}

template<class T> Modsat LinearPiece::binaryModsat(unsigned n, const T& logTerm)
{
    Modsat binMS;

    if ( n == 0 )
    {
        binMS.phi = zeroFormula();
        return binMS;
    }
    else if ( n == 1 )
    {
        binMS.phi = Formula(logTerm);
        return binMS;
    }
    else
    {
        Clause clau;

        binMS.Phi.push_back(Formula(Formula(var->newVariable()), Formula(logTerm), Equiv));
        if ( n & 1 )
            clau.push_back((Literal) var->currentVariable());

        for ( int i = 1; i <= ilogb(n); i++ )
        {
            Variable oldVar = var->currentVariable();
            binMS.Phi.push_back(Formula(Formula(var->newVariable()), Formula(Clause(2, oldVar)), Equiv));
            if ( n >> i & 1 )
                clau.push_back((Literal) var->currentVariable());
        }

        binMS.phi = Formula(clau);
        return binMS;
    }
}

// Define modsat a constant of type 1/denum by a propositional variable.
// It is enough to run once for each constant and then ask the variable manager for future use.
ModsatSet LinearPiece::defineConstant(unsigned denum)
{
    Modsat msAux = binaryModsat(denum-1, var->newConstant(denum));
    msAux.Phi.push_back(Formula(var->constant(denum), Formula(msAux.phi,Neg), Equiv));
    return msAux.Phi;
}

// Multiply by num _already defined_ constant 1/denum in order to refer to value of fraction num/denum.
// num must have at most the same number of bits as denum-1 OR num == denum
Modsat LinearPiece::multiplyConstant(unsigned num, unsigned denum)
{
    Modsat frac;

    if ( denum <= 2 )
        frac = binaryModsat(num, var->constant(denum));
    else if ( ilogb(num) <= ilogb(denum-1) )
    {
        for ( int i = 0; i <= ilogb(num); i++ )
            if ( num >> i & 1 )
                frac.phi.addLukaDisjunction(Formula(var->constant(denum)+i+1));
    }
    else if ( num == denum )
    {
        if ( !var->isThereAuxMultVariable(denum) )
            frac.Phi.push_back(Formula(Formula(var->auxMultVariable(denum)), Formula(Clause(2,var->constant(denum)+ilogb(num))), Equiv));

        frac.phi = Formula(var->auxMultVariable(denum));
    }

    return frac;
}

// Multiply a propositional variable for the second time.
// The propositional variable must have been multiplied by some integer m before.
// Multiplicative factor n must be at most m.
Formula LinearPiece::variableSecondMultiplication(unsigned n, Variable var)
{
    Clause auxClau;

    if ( n == 0 )
        return zeroFormula();
    else if ( n == 1 )
        auxClau.push_back(var);
    else
    {
        for ( int i = 0; i <= ilogb(n); i++ )
            if ( n >> i & 1 )
                auxClau.push_back(var+i+1);
    }

    return Formula(auxClau);
}

void LinearPiece::pwl2limodsat()
{
    bool allZeroCoefficients = true;

    for ( unsigned i = 0; i<=dim; i++ )
        if ( linearPieceData[i].first != 0 )
            allZeroCoefficients = false;

    if ( allZeroCoefficients )
    {
        Modsat zeroRepresentation;
        zeroRepresentation.phi = zeroFormula();
        representationModsat = zeroRepresentation;
    }
    else
    {
        enum Sign { P, N };

        std::vector<unsigned> alphas, betas;
        double betaPositive = 0, betaNegative = 0;
        std::vector<unsigned> indexes[2];

        for ( unsigned i = 0; i <= dim; i++ )
        {
            if ( linearPieceData[i].first >= 0 )
            {
                alphas.push_back(linearPieceData[i].first);
                betaPositive += (double) linearPieceData[i].first / (double) linearPieceData[i].second;
                indexes[P].push_back(i);
            }
            else
            {
                alphas.push_back(-linearPieceData[i].first);
                betaNegative += -((double) linearPieceData[i].first / (double) linearPieceData[i].second);
                indexes[N].push_back(i);
            }
        }

        unsigned beta = ceil(fmax(betaPositive, betaNegative));

        for ( unsigned i = 0; i <= dim; i++ )
            betas.push_back(linearPieceData[i].second * beta);

        Modsat representation[2];
        ModsatSet msSetAux;
        Modsat msAux;

        for ( Sign J : { P, N } )
        {
            bool zeroIndexes = true;

            for ( size_t j = 0; j < indexes[J].size(); j++ )
                if ( alphas.at(indexes[J].at(j)) != 0 )
                    zeroIndexes = false;

            if ( ( indexes[J].empty() ) || ( zeroIndexes ) )
                representation[J].phi = zeroFormula();
            else
            {
                if ( ( indexes[J].at(0) == 0 ) && ( alphas.at(indexes[J].at(0)) != 0 ) )
                {
                    if ( !var->isThereConstant(betas.at(0)) )
                    {
                        msSetAux = defineConstant(betas.at(0));
                        representation[J].Phi.insert(representation[J].Phi.end(), msSetAux.begin(), msSetAux.end());
                    }
                    msAux = multiplyConstant(alphas.at(0),betas.at(0));
                    representation[J].phi.addLukaDisjunction(msAux.phi);
                    representation[J].Phi.insert(representation[J].Phi.end(), msAux.Phi.begin(), msAux.Phi.end());
                }

                for ( size_t j = (indexes[J].at(0) == 0 ? 1 : 0); j < indexes[J].size(); j++ )
                {
                    if ( alphas.at(indexes[J].at(j)) != 0 )
                    {
                        if ( !var->isThereConstant( betas.at(indexes[J].at(j)) ) )
                        {
                            msSetAux = defineConstant(betas.at(indexes[J].at(j)));
                            representation[J].Phi.insert(representation[J].Phi.end(), msSetAux.begin(), msSetAux.end());
                        }

                        unsigned Max = fmax(alphas.at(indexes[J].at(j)), betas.at(indexes[J].at(j)));
                        unsigned Min = fmin(alphas.at(indexes[J].at(j)), betas.at(indexes[J].at(j)));

                        Variable auxVar = var->newVariable();
                        msAux = binaryModsat(Max, auxVar);
                        representation[J].Phi.insert(representation[J].Phi.end(), msAux.Phi.begin(), msAux.Phi.end());

                        if ( alphas.at(indexes[J].at(j)) == betas.at(indexes[J].at(j)) )
                        {
                            representation[J].phi.addLukaDisjunction(msAux.phi);
                            representation[J].Phi.push_back(Formula(msAux.phi, Formula(indexes[J].at(j)), Equiv));
                        }
                        else if ( alphas.at(indexes[J].at(j)) < betas.at(indexes[J].at(j)) )
                        {
                            representation[J].phi.addLukaDisjunction(variableSecondMultiplication(Min,auxVar));
                            representation[J].Phi.push_back(Formula(msAux.phi, Formula(indexes[J].at(j)), Equiv));
                        }
                        else
                        {
                            representation[J].phi.addLukaDisjunction(msAux.phi);
                            representation[J].Phi.push_back(Formula(variableSecondMultiplication(Min,auxVar), Formula(indexes[J].at(j)), Equiv));
                        }

                        representation[J].Phi.push_back(Formula(Formula(auxVar), Formula(var->constant(betas.at(indexes[J].at(j)))), Impl));
                    }
                }
            }
        }

        msAux = binaryModsat(beta, Formula(Formula(representation[P].phi, representation[N].phi, Impl), Neg));

        representationModsat.phi = msAux.phi;
        representationModsat.Phi = representation[P].Phi;
        representationModsat.Phi.insert(representationModsat.Phi.end(), representation[N].Phi.begin(), representation[N].Phi.end());
        representationModsat.Phi.insert(representationModsat.Phi.end(), msAux.Phi.begin(), msAux.Phi.end());
    }
}

void LinearPiece::representModsat()
{
    if ( !modsatTranslation )
        pwl2limodsat();

    modsatTranslation = true;
}

Modsat LinearPiece::getRepresentationModsat()
{
    if ( !modsatTranslation )
        representModsat();

    return representationModsat;
}

void LinearPiece::printModsatSet(std::ofstream *output)
{
    if ( !modsatTranslation )
        representModsat();

    for ( size_t i = 0; i < representationModsat.Phi.size(); i++ )
    {
        *output << "Formula " << i+1 << ":" << std::endl;
        representationModsat.Phi.at(i).print(output);
    }
}

void LinearPiece::printLimodsatFile()
{
    if ( !modsatTranslation )
        representModsat();

    std::ofstream outputFile(outputFileName);

    outputFile << "-= Formula phi =-" << std::endl << std::endl;
    representationModsat.phi.print(&outputFile);

    outputFile << std::endl << "-= MODSAT Set Phi =-" << std::endl << std::endl;

    printModsatSet(&outputFile);

    outputFile.close();
}
}
