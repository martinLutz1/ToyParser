#include "preparser.h"
#include "parsererror.h"
#include <QDebug>

PreParser::PreParser(QStringOperations *qStringOps, ParserData *parserData):
    qStringOps(qStringOps), parserData(parserData), amountOfVariables(0), amountOfLabels(0), amountSaveDataInstructions(0)
{
}

void PreParser::preParseProgram()
{
    int amountInstructions = countAmountInstructions();

    // First save_data instructions
    checkProgramForSaveDataInstruction();
    writeSaveDataInstructions(amountInstructions);

    // Second variables
    checkProgramForVariableDeclarations();
    replaceVariablesWithValues(amountInstructions);

    // Third lables
    checkProgramForLabelDeclarations();
    replaceLabelsWithValues();
}

void PreParser::resetPreParserStates()
{
    amountOfLabels = 0;
    amountOfVariables = 0;
    amountSaveDataInstructions = 0;
}

short PreParser::countAmountInstructions()
{
    short counter = 0;
    for (int i = 0; i < parserData->wholeProgram.length(); i++)
    {
        QString nextLineString = qStringOps->simplifyString(parserData->wholeProgram[i]);
        QRegularExpressionMatch matchDeclareVariable = parserData->declareVariableRegExp.match(nextLineString, 0, QRegularExpression::PartialPreferCompleteMatch);
        QRegularExpressionMatch matchDeclareLabel = parserData->declareLabelRegExp.match(nextLineString, 0, QRegularExpression::PartialPreferCompleteMatch);
        QRegularExpressionMatch matchSaveData = parserData->saveDataRegExp.match(nextLineString, 0, QRegularExpression::PartialPreferCompleteMatch);

        // Dont count them, as we only count standard instructions (hex, bin and mnemonic)
        if (matchDeclareLabel.hasMatch() || matchDeclareVariable.hasMatch() || matchSaveData.hasMatch() || nextLineString.length() == 0)
        {
            continue;
        }
        counter++;
    }
    return counter;
}

void PreParser::checkProgramForSaveDataInstruction()
{

    for (int lineNumber = 0; lineNumber < parserData->wholeProgram.length(); lineNumber++)
    {
        QString line = qStringOps->simplifyString(parserData->wholeProgram[lineNumber]);
        QRegularExpressionMatch matchSaveData = parserData->saveDataRegExp.match(line, 0, QRegularExpression::PartialPreferCompleteMatch);
        if (line.startsWith(":"))
        {
            QString addressText;
            QString valueText;
            QStringList saveList = line.split(":");

            if (saveList.size() == 3) // assign values from line to addressText and valueText as String
            {
                addressText = saveList[1];
                valueText = saveList[2];
                if (!matchSaveData.hasMatch()) // No match, just 2 (:)
                {
                    if (addressText.length() == 0)
                    {
                        QString errorMsg = "Fehler in Speicherbefehl \"" + parserData->wholeProgram[lineNumber]
                                + "\" in Zeile " + QString::number(lineNumber+1) + ": Keine Adresse vorhanden";
                        throw ParserError(errorMsg.toStdString());
                    }
                    else if (valueText.length() == 0)
                    {
                        QString errorMsg = "Fehler in Speicherbefehl \"" + parserData->wholeProgram[lineNumber]
                                + "\" in Zeile " + QString::number(lineNumber+1) + ": Kein zu speichernder Wert vorhanden";
                        throw ParserError(errorMsg.toStdString());
                    }
                    else // Something else wrong
                    {
                        QString errorMsg = "Fehler in Speicherbefehl \"" + parserData->wholeProgram[lineNumber]
                                + "\" in Zeile " + QString::number(lineNumber+1) + ": Fehler in Adresse oder in zu speicherndem Wert";
                        throw ParserError(errorMsg.toStdString());
                    }
                }
            }
            else if (saveList.size() < 3) // Not enough (:)
            {
                QString errorMsg = "Fehler in Speicherbefehl \"" + parserData->wholeProgram[lineNumber]
                        + "\" in Zeile " + QString::number(lineNumber+1) + ": zu wenige Doppelpunkte(:)";
                throw ParserError(errorMsg.toStdString());
            }
            else if (saveList.size() > 3) // Too many (:)
            {
                QString errorMsg = "Fehler in Speicherbefehl \"" + parserData->wholeProgram[lineNumber]
                        + "\" in Zeile " + QString::number(lineNumber+1) + ": zu viele Doppelpunkte(:)";
                throw ParserError(errorMsg.toStdString());
            }

            int addressNumberType = qStringOps->checkNumberType(addressText, lineNumber);
            int valueNumberType = qStringOps->checkNumberType(valueText, lineNumber);

            saveDataValue[amountSaveDataInstructions] = qStringOps->qStringToShort(valueText, valueNumberType);
            saveDataAdress[amountSaveDataInstructions] = qStringOps->qStringToShort(addressText, addressNumberType);
            saveDataLineNumber[amountSaveDataInstructions] = lineNumber;
            amountSaveDataInstructions++;
        }
    }
}

void PreParser::writeSaveDataInstructions(int amountAssignedInstructions)
{
    for (int i = 0; i < amountSaveDataInstructions; i++)
    {
        if (saveDataAdress[i] < RAMSIZE && saveDataAdress[i] >= amountAssignedInstructions) // Address fine
        { // Write value in "parser-RAM"
            parserData->parserRAM[saveDataAdress[i]] = saveDataValue[i];
            parserData->isValueInRAM[saveDataAdress[i]] = true;
        }
        else if (saveDataAdress[i] < amountAssignedInstructions) // Dont overwrite instructions in RAM
        {
            QString errorMsg = "Fehler in Speicherbefehl \"" + parserData->wholeProgram[saveDataLineNumber[i]]
                    + "\" in Zeile " + QString::number(saveDataLineNumber[i]+1)
                    + ": RAM an Adresse " + QString::number(saveDataAdress[i]) + " schon durch Instruktion belegt";
            throw ParserError(errorMsg.toStdString());
        }
        else // RAM address out of range
        {
            QString errorMsg = "Fehler in Speicherbefehl \"" + parserData->wholeProgram[saveDataLineNumber[i]]
                    + "\" in Zeile " + QString::number(saveDataLineNumber[i]+1) + ": ungültige RAM-Adresse";
            throw ParserError(errorMsg.toStdString());
        }
        parserData->wholeProgram[saveDataLineNumber[i]] = ""; // Remove declaration
    }
}

void PreParser::checkProgramForVariableDeclarations()
{
    for (int currentLine = 0; currentLine < parserData->wholeProgram.length(); currentLine++)
    {
        QString nextLineString = qStringOps->simplifyString(parserData->wholeProgram[currentLine]);
        QRegularExpressionMatch matchDeclareVariable = parserData->declareVariableRegExp.match(nextLineString, 0, QRegularExpression::PartialPreferCompleteMatch);

        if (matchDeclareVariable.hasMatch())
        {
            QStringList variable = nextLineString.split("=");
            if (variable.size() >= 1) // Shouldnt be required, since already checked for a match, just to be sure
            {
                if (variable[0].length() < 3) // To avoid replacing single letters
                {
                    QString errorMsg = "Variablen-Bezeichnung \"" + parserData->wholeProgram[currentLine] + "\" in Zeile "
                            + QString::number(currentLine + 1) + " zu kurz, muss mindestens 3 Zeichen lang sein";
                    throw ParserError(errorMsg.toStdString());
                }

                QRegularExpressionMatch matchMnemonic = parserData->mnemonicExactRegExp.match(variable[0], 0, QRegularExpression::PartialPreferCompleteMatch);

                if (matchMnemonic.hasMatch()) // Mnemonic detected in variable name
                {
                    QString errorMsg = "Variablen-Bezeichnung \"" + parserData->wholeProgram[currentLine] + "\" in Zeile "
                            + QString::number(currentLine + 1) + " ist Mnemonic-Bezeichner";
                    throw ParserError(errorMsg.toStdString());
                }

                int numberSystem = qStringOps->checkNumberType(variable[1], currentLine);
                variableName[amountOfVariables] = variable[0];
                variableValue[amountOfVariables] = qStringOps->qStringToShort(variable[1], numberSystem);
                amountOfVariables++;
            }
            parserData->wholeProgram[currentLine] = ""; // Remove declaration
        }
    }
}

void PreParser::replaceVariablesWithValues(int amountInstructions)
{
    for (int i = 0; i < amountOfVariables; i++)
    {
        short variablePosition = -1;

        for (int j = amountInstructions; j < RAMSIZE; j++) // Determine space in RAM for variable
        {
            if (!parserData->isValueInRAM[j]) // Free RAM
            {
                variablePosition = j;
                break;
            }
        }

        if (variablePosition == -1) // No free space found
        {
            QString errorMsg = "Für die Variable \"" + variableName[i] +
                    "\" konnte keine freie Stelle im Speicher gefunden werden. ";
            throw ParserError(errorMsg.toStdString());
        }
        else // Write variable value to variable position in RAM
        {
            parserData->parserRAM[variablePosition] = variableValue[i];
            parserData->isValueInRAM[variablePosition] = true;
        }

        QRegularExpression variableI;
        variableI.setPattern(variableName[i]);

        for (int currentLine = 0; currentLine < parserData->wholeProgram.length(); currentLine++)
        {
            QString nextLineString = qStringOps->simplifyString(parserData->wholeProgram[currentLine]);
            QRegularExpressionMatch matchVariableI = variableI.match(nextLineString, 0, QRegularExpression::PartialPreferCompleteMatch);

            if (matchVariableI.hasMatch()) // Variable name is found in instruction
            {
                QStringList instruction = qStringOps->splitQStringAt(nextLineString, matchVariableI.capturedStart()); // Cut instruction from variable
                nextLineString = instruction[0] + QString::number(variablePosition);
                parserData->wholeProgram[currentLine] = nextLineString;
            }
        }
    }
}

void PreParser::checkProgramForLabelDeclarations()
{
    int placeOfInstructions = 0; // Used to determine the address of the label
    for (int currentLine = 0; currentLine < parserData->wholeProgram.length(); currentLine++)
    {
        QString nextLineString = qStringOps->simplifyString(parserData->wholeProgram[currentLine]);

        QRegularExpressionMatch matchDeclareLabel = parserData->declareLabelRegExp.match(nextLineString, 0, QRegularExpression::PartialPreferCompleteMatch);
        if (matchDeclareLabel.hasMatch())
        {
            QStringList variable = nextLineString.split(":");
            if (variable.size() != 0) // Shouldnt be required, since already checked for a match
            {
                if (variable[0].length() < 3) // To avoid replacing single letters
                {
                    QString errorMsg = "Label \"" + parserData->wholeProgram[currentLine] + "\" in Zeile "
                            + QString::number(currentLine + 1) + " ist zu kurz, muss mindestens 3 Zeichen lang sein";
                    throw ParserError(errorMsg.toStdString());
                }
                QRegularExpressionMatch matchMnemonic = parserData->mnemonicExactRegExp.match(variable[0], 0, QRegularExpression::PartialPreferCompleteMatch);
                if (matchMnemonic.hasMatch()) // Mnemonic detected in label name
                {
                    QString errorMsg = "Label \"" + parserData->wholeProgram[currentLine] + "\" in Zeile "
                            + QString::number(currentLine + 1) + " ist Mnemonic-Bezeichner";
                    throw ParserError(errorMsg.toStdString());
                }

                labelName[amountOfLabels] = variable[0];
                labelPosition[amountOfLabels] = placeOfInstructions; // This line will be the one we spring to at brz-instruction
                amountOfLabels++;
            }
            parserData->wholeProgram[currentLine] = ""; // Remove declaration
        }
        if (nextLineString.length() != 0) // only increase if an actual instruction
        {
            placeOfInstructions++;
        }
    }
}

void PreParser::replaceLabelsWithValues()
{
    for (int i = 0; i < amountOfLabels; i++)
    {
        QRegularExpression labelI;
        labelI.setPattern(labelName[i]);

        for (int currentLine = 0; currentLine < parserData->wholeProgram.length(); currentLine++)
        {
            QString nextLineString = qStringOps->simplifyString(parserData->wholeProgram[currentLine]);
            QRegularExpressionMatch matchLableI = labelI.match(nextLineString, 0, QRegularExpression::PartialPreferCompleteMatch);

            if(matchLableI.hasMatch()) // Label found in instruction (usually BRZ)
            {
                QStringList instruction = qStringOps->splitQStringAt(nextLineString, matchLableI.capturedStart()); // Cut instruction from variable
                nextLineString = instruction[0] + QString::number(labelPosition[i]);
                parserData->wholeProgram[currentLine] = nextLineString;
            }
        }
    }
}
