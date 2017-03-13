#ifndef PARSERDATA_H
#define PARSERDATA_H

#include <QStringList>
#include <QRegularExpression>

#define BINARY 2
#define DECIMAL 10
#define HEXADECIMAL 16
#define MNEMONIC 3
#define RAMSIZE 4096

class ParserData
{
public:
    ParserData();

    QStringList wholeProgram;

    unsigned short parserRAM[RAMSIZE]; // Program-representation as it will be written to the RAM
    unsigned short parserBackupRAM[RAMSIZE]; // Holds last working RAM-state
    short amountOfInstructions; // Total amount of instruction, used as length of programAsShort
    bool isValueInRAM[RAMSIZE]; // true if instruction at programAsShort[index], false otherwise
    bool isValueInRAMBackup[RAMSIZE];  // Holds last working RAM-state

    QRegularExpression binaryRegExp;
    QRegularExpression decimalRegExp;
    QRegularExpression hexadecimalRegExp;
    QRegularExpression mnemonicRegExp;
    QRegularExpression mnemonicExactRegExp; // Describes mnemonics without any arguments
    QRegularExpression saveDataRegExp;
    QRegularExpression declareVariableRegExp;
    QRegularExpression declareLabelRegExp;
};

#endif // PARSERDATA_H
