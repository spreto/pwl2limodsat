#include <stdexcept>

#include "InputParser.h"

using namespace std;

InputParser::InputParser(const char* iFN) :
    inputFileName(iFN)
{
    inputFile.open(inputFileName);

    if ( !inputFile.is_open() )
        throw invalid_argument("Unable to open input file.");
    else
    {
        nextLine();
        if ( currentLine.compare(0,3,"tl ") == 0)
            mode = TL;
        else if ( currentLine.compare(0,3,"pwl") == 0 )
            mode = PWL;
        else
            throw invalid_argument("Not in standard pwl file format.");
    }
}

InputParser::~InputParser()
{
    inputFile.close();
}

void InputParser::nextLine()
{
    getline(inputFile,currentLine);

    while ( ( ( currentLine.compare(0,1,"c") == 0 ) || ( currentLine.empty() ) ) && !inputFile.eof() )
        getline(inputFile, currentLine);
}

vector<LinearPieceCoefficient> InputParser::readLinearPiece(unsigned beginingPosition)
{
    vector<LinearPieceCoefficient> coefs;
    size_t beginPosition = 0, endPosition;
    int numerator, denumerator;

    do
    {
        beginPosition = ( beginPosition == 0 ? beginingPosition : endPosition+1 );
        endPosition = currentLine.find_first_of(" ", beginPosition);

        if ( (beginPosition > currentLine.size()) || (endPosition > currentLine.size()) )
            throw invalid_argument("Not in standard pwl file format.");
        else
        {
            numerator = stoi( currentLine.substr( beginPosition, endPosition-beginPosition ) );
            beginPosition = endPosition+1;
            endPosition = currentLine.find_first_of(" ", beginPosition);
        }
        if ( beginPosition > currentLine.size() )
            throw invalid_argument("Not in standard pwl file format.");
        else
        {
            denumerator = stoi( currentLine.substr( beginPosition, endPosition-beginPosition ) );
            if ( denumerator < 1 )
                throw out_of_range("Fraction denumerator must be positive.");
            else
                coefs.push_back( LinearPieceCoefficient( numerator, (unsigned) denumerator ) );
        }
    } while ( endPosition < currentLine.size() );

    return coefs;
}

LinearPiece InputParser::createTLInstance()
{
    return LinearPiece(readLinearPiece(3), inputFileName);
}

PiecewiseLinearFunction InputParser::createPWLInstance()
{
    vector<BoundaryPrototype> boundProts;
    vector<vector<LinearPieceCoefficient>> coefss;
    vector<vector<Boundary>> boundss;

    BoundaryPrototype boundProt;
    vector<LinearPieceCoefficient> coefs;
    vector<Boundary> bounds;

    size_t beginPosition = 0, endPosition;
    unsigned boundaryCounter = 0;
    unsigned boundaryNumber;

    nextLine();

    while ( !inputFile.eof() )
    {
        if ( currentLine.compare(0,2,"b ") == 0 )
        {
            do
            {
                beginPosition = ( beginPosition == 0 ? 2 : endPosition+1 );
                endPosition = currentLine.find_first_of(" ", beginPosition);

                if ( beginPosition > currentLine.size() )
                    throw invalid_argument("Not in standard pwl file format.");
                else
                    boundProt.push_back(stof(currentLine.substr(beginPosition, endPosition-beginPosition)));
            } while ( endPosition < currentLine.size() );

            boundProts.push_back(boundProt);
            boundProt.clear();
            boundaryCounter++;
            beginPosition = 0;

            nextLine();
        }
        else if ( currentLine.compare(0,2,"p ") == 0 )
        {
            coefs = readLinearPiece(2);
            nextLine();

            while ( (currentLine.compare(0,2,"g ") == 0) || (currentLine.compare(0,2,"l ") == 0) )
            {
                if ( currentLine.size() < 3 )
                    throw invalid_argument("Not in standard pwl file format.");
                else
                    boundaryNumber = (unsigned) stoul(currentLine.substr(2, currentLine.size()-2));

                if ( (boundaryNumber > 0) && (boundaryNumber <= boundaryCounter) )
                {
                    if ( currentLine.compare(0,2,"g ") == 0 )
                        bounds.push_back(pair<unsigned,BoundarySymbol>(boundaryNumber, GeqZero));
                    else if ( currentLine.compare(0,2,"l ") == 0 )
                        bounds.push_back(pair<unsigned,BoundarySymbol>(boundaryNumber, LeqZero));
                }
                else
                    throw out_of_range("Nonexistent boundary prototype.");

                nextLine();
            }

            boundss.push_back(bounds);
            coefss.push_back(coefs);
            bounds.clear();
            coefs.clear();
        }
        else
            throw invalid_argument("Not in standard pwl file format.");
    }

    unsigned dim = boundProts.at(0).size();

    for ( auto i = 1; i < boundProts.size(); i++ )
        if ( boundProts.at(i).size() != dim )
            throw invalid_argument("Dimension inconsistency.");

    for ( auto i = 0; i < coefss.size(); i++ )
        if ( coefss.at(i).size() != dim )
            throw invalid_argument("Dimension inconsistency.");

    return PiecewiseLinearFunction(coefss, boundss, boundProts, inputFileName);
}
