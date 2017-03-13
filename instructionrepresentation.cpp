#include "instructionrepresentation.h"
#include "parsererror.cpp"
#include "parserdata.h"
#include "QStringList"
#include "QDebug"

// We dont check for correct input, since every unsigned short is fine

InstructionRepresentation::InstructionRepresentation()
{
}

QString InstructionRepresentation::asBinary(unsigned short instruction)
{
    QString binaryRepresentation = QString::number(instruction, 2); // Convert short to binary
    while (binaryRepresentation.length() < 16) // Fill with zeros from left to get 16 bit
    {
        binaryRepresentation = "0" + binaryRepresentation;
    }

    QString binaryRepresentationWithSpaces = "";
    for (int i = 0; i < 16; i++)
    {
        binaryRepresentationWithSpaces += binaryRepresentation.at(i);

        if ((i+1) % 4 == 0)
            binaryRepresentationWithSpaces += " ";
    }

    return binaryRepresentationWithSpaces.simplified();
}

QString InstructionRepresentation::asHex(unsigned short instruction)
{
    QString hexadecimalRepresentation = QString::number(instruction, 16); // Convert short to hexadecimal
    while (hexadecimalRepresentation.length() < 4) // Fill with zeros from left to get 4 bytes
    {
        hexadecimalRepresentation = "0" + hexadecimalRepresentation;
    }
    return "$" + hexadecimalRepresentation.toUpper();
}

QString InstructionRepresentation::asMnemonic(unsigned short instruction, int numberType)
{
    QStringList list = asMnemonicSplitted(instruction, numberType);
    return list[0] + " " + list[1];
}

QStringList InstructionRepresentation::asMnemonicSplitted(unsigned short instruction, int numberType)
{
    unsigned short mnemonicShort = instruction >> 12; // Mnemonic is the 4 bits from left
    unsigned short numberShort = instruction - (mnemonicShort << 12);
    bool mnemonicWithoutNumber = false; // Used to identify mnemonics without any numbers / arguments
    QString mnemonic;
    QString number;

    switch (mnemonicShort)
    {
    case 0:
        mnemonic = "STO";
        break;
    case 1:
        mnemonic = "LDA";
        break;
    case 2:
        mnemonic = "BRZ";
        break;
    case 3:
        mnemonic = "ADD";
        break;
    case 4:
        mnemonic = "SUB";
        break;
    case 5:
        mnemonic = "OR";
        break;
    case 6:
        mnemonic = "AND";
        break;
    case 7:
        mnemonic = "XOR";
        break;
    case 8:
        mnemonic = "NOT";
        mnemonicWithoutNumber = true;
        break;
    case 9:
        mnemonic = "INC";
        mnemonicWithoutNumber = true;
        break;
    case 10:
        mnemonic = "DEC";
        mnemonicWithoutNumber = true;
        break;
    case 11:
        mnemonic = "ZRO";
        mnemonicWithoutNumber = true;
        break;
    case 12:
    case 13:
    case 14:
    case 15:
        mnemonic = "NOP";
        mnemonicWithoutNumber = true;
        break;
    }

    if (mnemonicWithoutNumber)
    {
        QStringList result;
        result << mnemonic << "";
        return result;
    }
    else // Mnemonics with argument / number
    {
        QString number = QString::number(numberShort, numberType); // Convert short to binary, hexadecimal or decimal
        if (numberType == HEXADECIMAL)
        {
            number = "$" + number.toUpper();
        }
        QStringList result;
        result << mnemonic << number;
        return result;
    }

}
