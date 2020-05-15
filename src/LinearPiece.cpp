#include <cmath>

#include "LinearPiece.h"

LinearPiece::LinearPiece(const vector<LinearPieceCoefficient>& coefs, VariableManager *varMan) :
    pieceCoefficients(coefs),
    dim(pieceCoefficients.size()-1),
    var(varMan) {}

LinearPiece::LinearPiece(const vector<LinearPieceCoefficient>& coefs, string inputFileName) :
    pieceCoefficients(coefs),
    dim(pieceCoefficients.size()-1)
{
    var = new VariableManager(dim);
    ownVariableManager = true;

    if ( inputFileName.substr(inputFileName.size()-4,4) == ".pwl" )
        outputFileName = inputFileName.substr(0,inputFileName.size()-4);
    else
        outputFileName = inputFileName;

    outputFileName.append(".out");
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

template<class T>
ModSat LinearPiece::binaryModSat(unsigned n, const T& logTerm)
{
    ModSat binMS;

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

// Define modsat a constant of type 1/denum by a propositional variable.
// It is enough to run once for each constant and then ask the variable manager for future use.
ModSatSet LinearPiece::defineConstant(unsigned denum)
{
    ModSat msAux = binaryModSat(denum-1, var->newConstant(denum));
    msAux.Phi.push_back(Formula(var->constant(denum), Formula(msAux.phi,Neg), Equiv));
    return msAux.Phi;
}

// Multiply by num _already defined_ constant 1/denum in order to refer to value of fraction num/denum.
// num must have at most the same number of bits as denum-1 OR num == denum
ModSat LinearPiece::multiplyConstant(unsigned num, unsigned denum)
{
    ModSat frac;

    if ( denum <= 2 )
        frac = binaryModSat(num, var->constant(denum));
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

void LinearPiece::representModSat()
{
    bool allZeroCoefficients = true;

    for ( auto i = 0; i<=dim; i++ )
        if ( pieceCoefficients[i].first != 0 )
            allZeroCoefficients = false;

    if ( allZeroCoefficients )
    {
        ModSat zeroRepresentation;
        zeroRepresentation.phi = zeroFormula();
        representationModSat = zeroRepresentation;
    }
    else
    {
        enum Sign { P, N };

        vector<unsigned> alphas, betas;
        vector<unsigned> indexes[2];

        for ( auto i = 0; i <= dim; i++ )
        {
            if ( pieceCoefficients[i].first >= 0 )
            {
                alphas.push_back(pieceCoefficients[i].first);
                indexes[P].push_back(i);
            }
            else
            {
                alphas.push_back(-pieceCoefficients[i].first);
                indexes[N].push_back(i);
            }
        }

        unsigned beta = ceil(abs((double) pieceCoefficients[0].first / (double) pieceCoefficients[0].second));
        for ( auto i = 1; i <= dim; i++ )
            if ( beta < ceil(abs((double) pieceCoefficients[i].first / (double) pieceCoefficients[i].second)) )
                beta = ceil(abs((double) pieceCoefficients[i].first / (double) pieceCoefficients[i].second));

        for ( auto i = 0; i <= dim; i++ )
            betas.push_back(pieceCoefficients[i].second * beta);

        ModSat representation[2];
        ModSatSet msSetAux;
        ModSat msAux;

        for ( Sign J : { P, N } )
        {
            bool zeroIndexes = true;

            for ( auto j = 0; j < indexes[J].size(); j++ )
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

                for ( auto j = (indexes[J].at(0) == 0 ? 1 : 0); j < indexes[J].size(); j++ )
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
                        msAux = binaryModSat(Max, auxVar);
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

        msAux = binaryModSat(beta, Formula(Formula(representation[P].phi, representation[N].phi, Impl), Neg));

        representationModSat.phi = msAux.phi;
        representationModSat.Phi = representation[P].Phi;
        representationModSat.Phi.insert(representationModSat.Phi.end(), representation[N].Phi.begin(), representation[N].Phi.end());
        representationModSat.Phi.insert(representationModSat.Phi.end(), msAux.Phi.begin(), msAux.Phi.end());
    }
}

ModSat LinearPiece::getRepresentationModSat()
{
    return representationModSat;
}

void LinearPiece::printModSatSet(ofstream *output)
{
    for ( auto i = 0; i < representationModSat.Phi.size(); i++ )
    {
        *output << "Formula " << i+1 << ":" << endl;
        representationModSat.Phi.at(i).print(output);
    }
}

void LinearPiece::printRepresentation()
{
    ofstream outputFile(outputFileName);

    outputFile << "-= Formula phi =-" << endl << endl;
    representationModSat.phi.print(&outputFile);

    outputFile << endl << "-= MODSAT Set Phi =-" << endl << endl;

    printModSatSet(&outputFile);

    outputFile.close();
}
