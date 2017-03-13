#include "parser.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "parsererror.h"

Parser::Parser () : qStringOps(&parserData), preParser(&qStringOps, &parserData)
{
}

bool Parser::readProgramText(QString toyCode)
{
    resetParserStates();

    if (!toyCode.isEmpty())
    {
        QTextStream in(&toyCode);
        while (!in.atEnd()) // Reads the text line by line and writes to wholeProgram
        {
            QString line = in.readLine();
            parserData.wholeProgram.push_back(line);
        }
        return true;
    }
    else // Empty text
    {
        return false;
    }
}

bool Parser::readProgramFile (QString filename)
{
    resetParserStates();

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        while (!in.atEnd()) // Reads the file line by line and writes to wholeProgram
        {
            QString line = in.readLine();
            parserData.wholeProgram.push_back(line);
        }
        file.close();
        return true;
    }
    else // Reading failed
    {
        return false;
    }
}

bool Parser::parseProgram () {
    try
    {
        preParser.preParseProgram(); // Deletes labels, variables and save_data instructions (:adress:value) and writes to RAM
        short nextInstruction;
        for (int currentLine = 0; currentLine < parserData.wholeProgram.length(); currentLine++)
        {
            QString nextLineString = qStringOps.simplifyString(parserData.wholeProgram[currentLine]);
            if (nextLineString.length() == 0) // Skip empty lines
            {
                continue;
            }

            QRegularExpression source;
            int sourceType = checkInputType(nextLineString, currentLine); // Determine input type
            switch(sourceType)
            {
            case BINARY:
                source = parserData.binaryRegExp;
                break;
            case HEXADECIMAL:
                source = parserData.hexadecimalRegExp;
                break;
            case MNEMONIC:
                source = parserData.mnemonicRegExp;
                break;
            }
            nextInstruction = getInstruction(sourceType, source, nextLineString, currentLine);

            parserData.parserRAM[parserData.amountOfInstructions] = nextInstruction; // Save instruction
            parserData.isValueInRAM[parserData.amountOfInstructions] = true; // Set flag that instruction is set in RAM
            parserData.amountOfInstructions++;
        }
        checkForInfinityLoop();
    }
    catch (ParserError &e)
    {
        QString error = e.what();
        error = error.simplified();
        Message msg;
        msg.type = Message::parser_parser_error_message;
        msg.data[0].stringPointer = &error;
        emit sendMessage(msg);

        loadRAMBackup();
        return false;
    }

    Message msgParsingComplete;
    msgParsingComplete.type = Message::parser_parsing_complete;
    emit sendMessage(msgParsingComplete);

    Message msgAmountInstructions;
    msgAmountInstructions.type = Message::parser_amount_instructions;
    msgAmountInstructions.data[0].us = parserData.amountOfInstructions;
    emit sendMessage(msgAmountInstructions);

    return true;
}

QString Parser::getProgram(int type)
{
    QString outputText;

    switch (type)
    {
    case reprType::binary:
    {
        for (int i = 0; i < RAMSIZE; i++)
        {
            if (parserData.isValueInRAM[i])
            {
                outputText += instrRepr.asBinary(parserData.parserRAM[i]);
                outputText += "\n";
            }
        }
        break;
    }
    case reprType::decimal:
    {
        for (int i = 0; i < RAMSIZE; i++)
        {
            if (parserData.isValueInRAM[i])
            {
                outputText += QString::number(parserData.parserRAM[i]);
                outputText += "\n";
            }
        }
        break;
    }
    case reprType::hexadecimal:
    {
        for (int i = 0; i < RAMSIZE; i++)
        {
            if (parserData.isValueInRAM[i])
            {
                outputText += instrRepr.asHex(parserData.parserRAM[i]);
                outputText += "\n";
            }
        }
        break;
    }
    case reprType::mnemonic:
    {
        for (int i = 0; i < RAMSIZE; i++)
        {
            if (parserData.isValueInRAM[i])
            {
                outputText += instrRepr.asMnemonic(parserData.parserRAM[i]);
                outputText += "\n";
            }
        }
        break;
    }
    default:
        break;
    }
    return outputText;
}

void Parser::resetParserStates()
{
    parserData.wholeProgram.clear(); // Delete program array
    memset(parserData.isValueInRAM, false, sizeof(parserData.isValueInRAM)); // Reset flags of parserRam
    parserData.amountOfInstructions = 0;

    preParser.resetPreParserStates();
}

int Parser::checkInputType(QString line, int lineNumber)
{
    QRegularExpressionMatch matchMnemonic = parserData.mnemonicRegExp.match(line, 0, QRegularExpression::PartialPreferCompleteMatch);

    if (matchMnemonic.hasMatch())
    {
        return MNEMONIC;
    }
    else if ((line.startsWith("0") || line.startsWith("1")) && line.length() >= 12)
    {
        return BINARY;
    }
    else if (line.startsWith("$"))
    {
        return  HEXADECIMAL;
    }
    else { // Type is unknown
        QString errorMsg = "Eingabetyp von \"" + parserData.wholeProgram[lineNumber]
                + "\" konnte nicht festgestellt werden in Zeile " + QString::number(lineNumber+1);
        throw ParserError(errorMsg.toStdString());
    }
}

short Parser::getInstruction(int sourceType, QRegularExpression source, QString line, int lineNumber)
{
    if (sourceType == MNEMONIC)
    {
        return getInstructionFromMnemonic(line, lineNumber);
    }

    QRegularExpressionMatch matchSource = source.match(line, 0, QRegularExpression::PartialPreferCompleteMatch);
    int length;
    switch (sourceType) // Set length of instruction
    {
    case BINARY:
        length = 16;
        break;

    case HEXADECIMAL:
        length = 4;
        break;
    }

    if (matchSource.hasMatch()) // Dont do this without match
    {
        return qStringOps.qStringToShort(line, sourceType);
    }
    else // Instruction contains mistakes
    {
        QString errorMsg;
        if (line.length() == length)
        {
            errorMsg = "Fehler in Instruktion \"" + parserData.wholeProgram[lineNumber]
                    + "\" in Zeile " + QString::number(lineNumber+1) + ": enthält unzulässiges Zeichen";
        }
        else if (line.length() < length)
        {
            errorMsg = "Fehler in Instruktion \"" + parserData.wholeProgram[lineNumber]
                    + "\" in Zeile " + QString::number(lineNumber+1) + ": zu kurz";
        }
        else if (line.length() > length)
        {
            errorMsg = "Fehler in Instruktion \"" + parserData.wholeProgram[lineNumber]
                    + "\" in Zeile " + QString::number(lineNumber+1) +  ": zu lang";
        }
        throw ParserError(errorMsg.toStdString());
    }
}

short Parser::getInstructionFromMnemonic(QString line, int lineNumber)
{
    QRegularExpressionMatch matchMnemonic = parserData.mnemonicRegExp.match(line, 0, QRegularExpression::PartialPreferCompleteMatch);
    if(!matchMnemonic.hasMatch()) // Spellcheck to ignore e.g. "XORE"
    {
        QString errorMsg = "Fehlerhaftes Mnemonic \"" + parserData.wholeProgram[lineNumber]
                + "\" in Zeile " + QString::number(lineNumber+1);
        throw ParserError(errorMsg.toStdString());
    }

    //----------------------- No errors, here we go ------------------------//
    QRegularExpression number("([0-9]|\\$)");
    QRegularExpressionMatch numberMatch;
    int separator = 0;
    short result = 0;

    for (int i = 0; i < line.length(); i++) { // Find number in mnemonic that will be the separator
        numberMatch = number.match((QString)(line[i]), 0, QRegularExpression::PartialPreferCompleteMatch);
        if (numberMatch.hasMatch())
        {
            separator = i; // Is the index of the first number in the instruction
            break;
        }
    }
    QStringList split; // split[0] has Mnemonic (e.g LDA), split[1] has number of Mnemonic (e.g. 34)
    if(separator > 0)
    {
        split = qStringOps.splitQStringAt(line, separator);
    }
    else // no numbers found and nothing to split, push whole line into split[0]
    {
        split << line << ""; // << "" is just used to have split[1], so we dont get any errors if we access split[1]
    }

    // If no argument (number), splitting goes wrong. So we fix it here
    if (line == "NOT" || line == "INC" || line == "DEC" || line == "ZRO" || line == "NOP")
    {
        split[0] = line;
    }

    if (split.size() != 0 && split[0].length() != 0) // Array split is not empty and Mnemonic is existing
    {
        if(split[0] == "STO")
        {
            result += 0;
        }
        else if (split[0] == "LDA")
        {
            result += 1;
        }
        else if(split[0] == "BRZ")
        {
            result += 2;
        }
        else if (split[0] == "ADD")
        {
            result += 3;
        }
        else if(split[0] == "SUB")
        {
            result += 4;
        }
        else if (split[0] == "OR")
        {
            result += 5;
        }
        else if(split[0] == "AND")
        {
            result += 6;
        }
        else if (split[0] == "XOR")
        {
            result += 7;
        }
        else if(split[0] == "NOT")
        {
            result += 8;
        }
        else if (split[0] == "INC")
        {
            result += 9;
        }
        else if(split[0] == "DEC")
        {
            result += 10;
        }
        else if (split[0] == "ZRO")
        {
            result += 11;
        }
        else if (split[0] == "NOP") // Is 12-15
        {
            result += 15;
        }
        result = result << 12; // Shifts 12 bits to left, because these are the 4 most significant bits
    }

    // These Mnemonics dont get any argument, so exclude them
    if (line != "NOT" && line != "INC" && line != "DEC" && line != "ZRO" && line != "NOP")
    {
        if (split[1].length() != 0) {
            int sourceType = qStringOps.checkNumberType(split[1], lineNumber); // Throws error if not determined, so no error handling here
            short numberOfMnemonic = qStringOps.qStringToShort(split[1], sourceType);

            if (numberOfMnemonic >= 0 && numberOfMnemonic < RAMSIZE)
            {
                result += numberOfMnemonic;
            }
            else // No number in range
            {
                QString errorMsg = "Fehler in Mnemonic \"" + parserData.wholeProgram[lineNumber]
                        + "\" in Zeile " + QString::number(lineNumber+1) + ": ungültige RAM-Adresse";
                throw ParserError(errorMsg.toStdString());
            }
        }
    }
    return result;
}

void Parser::checkForInfinityLoop()
{
    bool zroFound = (parserData.parserRAM[parserData.amountOfInstructions - 2] == (11 << 12));
    bool brzFound = (parserData.parserRAM[parserData.amountOfInstructions - 1] == (2 << 12) + (parserData.amountOfInstructions - 2));

    if (!(zroFound && brzFound))
    {
        QString errorMsg = "Keine Endlosschleife am Ende des Programms!";
        throw ParserError(errorMsg.toStdString());
    }
}

void Parser::backupRAM()
{
    memcpy(parserData.parserBackupRAM, parserData.parserRAM, sizeof(parserData.parserBackupRAM));
    memcpy(parserData.isValueInRAMBackup, parserData.isValueInRAM, sizeof(parserData.isValueInRAMBackup));
}

void Parser::loadRAMBackup()
{
    memcpy(parserData.parserRAM, parserData.parserBackupRAM, sizeof(parserData.parserRAM));
    memcpy(parserData.isValueInRAM, parserData.isValueInRAMBackup, sizeof(parserData.isValueInRAM));
}
