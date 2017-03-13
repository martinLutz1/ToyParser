#ifndef QSTRINGOPERATIONS_H
#define QSTRINGOPERATIONS_H

#include "parserdata.h"

class QStringOperations
{
public:
    QStringOperations(ParserData *parserData);

    ParserData *parserData;

    // Removes all white spaces, tabs "\t" and comments. Also puts all letters to capitals
    QString simplifyString(QString line);

    // Splits string at position and returns the result
    QStringList splitQStringAt(QString string, int position);

    // Returns the numeric value of the string in the given numberSystem, only use if checked for right format before (e.g. no "23Mue-")
    short qStringToShort(QString string, int numberSystem);

    // Returns BINARY(2), DECIMAL(10) or HEXADECIMAL(16)
    int checkNumberType(QString line, int lineNumber);
};

#endif // QSTRINGOPERATIONS_H
