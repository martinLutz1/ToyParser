#include "qstringoperations.h"
#include "parsererror.h"

QStringOperations::QStringOperations(ParserData *parserData) : parserData(parserData)
{
}

QString QStringOperations::simplifyString (QString line)
{
    line = line.simplified();
    line.replace(' ', "");
    QStringList split = line.split("#"); // Remove comment, relevant String in split[0]
    if (split.size() != 0 && split[0].length() != 0)
    {
        return split[0].toUpper();
    }
    return ""; // Should happen only when string empty or only a comment
}

QStringList QStringOperations::splitQStringAt(QString string, int position)
{
    QStringList twoStrings;
    twoStrings << "" << ""; // Create two strings

    if (position < string.length())
    {
        for (int i = 0; i < position; i++)
        {
            twoStrings[0] += string[i];
        }
        for (int i = position; i < string.length(); i++)
        {
            twoStrings[1] += string[i];
        }
    }
    return twoStrings;
}

short QStringOperations::qStringToShort(QString string, int numberSystem)
{
    if (numberSystem == 16) // Hexadecimal, remove "$"-symbol
    {
        QStringList hex = splitQStringAt(string, 1);
        string = hex[1];
    }
    bool conversionOk;
    short numericValue = string.toUShort(&conversionOk, numberSystem); // Converts "sourcetype" string (e.g. binary) to short
    if (!conversionOk) // Should never happen!
    {
        throw ParserError("Fehler! Soltte nicht passieren, sorry :(");
    }
    return numericValue;
}

int QStringOperations::checkNumberType(QString line, int lineNumber)
{
    QRegularExpressionMatch matchBinary = parserData->binaryRegExp.match(line, 0, QRegularExpression::PartialPreferCompleteMatch);
    QRegularExpressionMatch matchDecimal = parserData->decimalRegExp.match(line, 0, QRegularExpression::PartialPreferCompleteMatch);
    QRegularExpressionMatch matchHexadecimal = parserData->hexadecimalRegExp.match(line, 0, QRegularExpression::PartialPreferCompleteMatch);

    if (matchBinary.hasMatch())
    {
        return BINARY;
    }
    else if(matchDecimal.hasMatch())
    {
        return DECIMAL;
    }
    else if (line.startsWith("$") && matchHexadecimal.hasMatch())
    {
        return HEXADECIMAL;
    }
    else // Input type cannot be determined
    {
        QString errorMsg = "Zahlentyp von \"" + parserData->wholeProgram[lineNumber]
                + "\" konnte nicht festgestellt werden in Zeile " + QString::number(lineNumber+1);
        throw ParserError(errorMsg.toStdString());
    }
}
