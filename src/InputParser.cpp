#include <stdexcept>

#include "InputParser.h"

namespace pwl2limodsat
{
InputParser::InputParser(const char* iFN) :
    inputFileName(iFN)
{
    inputFile.open(inputFileName);

    if ( !inputFile.is_open() )
        throw std::invalid_argument("Unable to open input file.");
    else
    {
        nextLine();
        if ( currentLine.compare(0,3,"tl ") == 0)
            mode = TL;
        else if ( currentLine.compare(0,3,"pwl") == 0 )
            mode = PWL;
        else
            throw std::invalid_argument("Not in standard pwl file format.");
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

LinearPieceData InputParser::readLinearPiece(unsigned beginingPosition)
{
    LinearPieceData lpData;
    size_t beginPosition = 0, endPosition;
    LPCoefInteger numerator;
    LPCoefNonNegative denumerator;

    do
    {
        beginPosition = ( beginPosition == 0 ? beginingPosition : endPosition+1 );
        endPosition = currentLine.find_first_of(" ", beginPosition);

        if ( (beginPosition > currentLine.size()) || (endPosition > currentLine.size()) )
            throw std::invalid_argument("Not in standard pwl file format.");
        else
        {
            numerator = stoi( currentLine.substr( beginPosition, endPosition-beginPosition ) );
            beginPosition = endPosition+1;
            endPosition = currentLine.find_first_of(" ", beginPosition);
        }
        if ( beginPosition > currentLine.size() )
            throw std::invalid_argument("Not in standard pwl file format.");
        else
        {
            denumerator = stoi( currentLine.substr( beginPosition, endPosition-beginPosition ) );
            if ( denumerator < 1 )
                throw std::out_of_range("Fraction denumerator must be positive.");
            else
                lpData.push_back( LinearPieceCoefficient( numerator, (unsigned) denumerator ) );
        }
    } while ( endPosition < currentLine.size() );

    return lpData;
}

LinearPieceData InputParser::getTLInstanceData()
{
    return readLinearPiece(3);
}

void InputParser::buildPWLInstance()
{
    BoundaryPrototypeCollection boundProtCollection;
    PiecewiseLinearFunctionData pwlData;

    BoundaryPrototype boundProt;
    LinearPieceData lpData;
    BoundaryCollection boundaryCollection;

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
                    throw std::invalid_argument("Not in standard pwl file format.");
                else
                    boundProt.push_back(stof(currentLine.substr(beginPosition, endPosition-beginPosition)));
            } while ( endPosition < currentLine.size() );

            boundProtCollection.push_back(boundProt);
            boundProt.clear();
            boundaryCounter++;
            beginPosition = 0;

            nextLine();
        }
        else if ( currentLine.compare(0,2,"p ") == 0 )
        {
            lpData = readLinearPiece(2);
            nextLine();

            while ( (currentLine.compare(0,2,"g ") == 0) || (currentLine.compare(0,2,"l ") == 0) )
            {
                if ( currentLine.size() < 3 )
                    throw std::invalid_argument("Not in standard pwl file format.");
                else
                    boundaryNumber = (unsigned) stoul(currentLine.substr(2, currentLine.size()-2));

                if ( (boundaryNumber > 0) && (boundaryNumber <= boundaryCounter) )
                {
                    if ( currentLine.compare(0,2,"g ") == 0 )
                        boundaryCollection.push_back(Boundary(boundaryNumber-1, GeqZero));
                    else if ( currentLine.compare(0,2,"l ") == 0 )
                        boundaryCollection.push_back(Boundary(boundaryNumber-1, LeqZero));
                }
                else
                    throw std::out_of_range("Nonexistent boundary prototype.");

                nextLine();
            }

            RegionalLinearPieceData rlpData = { lpData, boundaryCollection };
            pwlData.push_back(rlpData);
            lpData.clear();
            boundaryCollection.clear();
        }
        else
            throw std::invalid_argument("Not in standard pwl file format.");
    }

    unsigned dim = boundProtCollection.at(0).size();

    for ( size_t i = 1; i < boundProtCollection.size(); i++ )
        if ( boundProtCollection.at(i).size() != dim )
            throw std::invalid_argument("Dimension inconsistency.");

    for ( size_t i = 0; i < pwlData.size(); i++ )
        if ( pwlData.at(i).lpData.size() != dim )
            throw std::invalid_argument("Dimension inconsistency.");

    pwlInstanceData = pwlData;
    pwlInstanceBoundProt = boundProtCollection;

    pwlTranslation = true;
}

PiecewiseLinearFunctionData InputParser::getPWLInstanceData()
{
    if ( !pwlTranslation )
        buildPWLInstance();

    return pwlInstanceData;
}

BoundaryPrototypeCollection InputParser::getPWLInstanceBoundProt()
{
    if ( !pwlTranslation )
        buildPWLInstance();

    return pwlInstanceBoundProt;
}
}
