#include "parserdata.h"

ParserData::ParserData(): amountOfInstructions(0)
{
    memset(isValueInRAM, false, sizeof(isValueInRAM));
    memset(parserRAM, 0, sizeof(parserRAM));
    memset(parserBackupRAM, 0, sizeof(parserBackupRAM));

    binaryRegExp.setPattern("^[0-1]{16}$");
    decimalRegExp.setPattern("^[0-9]*$");
    hexadecimalRegExp.setPattern("^\\$[0-9a-fA-F]*$");
    mnemonicRegExp.setPattern("^(STO|LDA|BRZ|ADD|SUB|OR|AND|XOR|NOT|INC|DEC|ZRO|NOP)\\$*[0-9a-fA-F]*$");
    mnemonicExactRegExp.setPattern("^(STO|LDA|BRZ|ADD|SUB|OR|AND|XOR|NOT|INC|DEC|ZRO|NOP)$");
    saveDataRegExp.setPattern("^:\\$*[0-9a-fA-F]+:\\$*[0-9a-fA-F]+$");
    declareVariableRegExp.setPattern("^[0-9a-zA-Z]+=\\$*[0-9a-fA-F]+$");
    declareLabelRegExp.setPattern("^[0-9a-zA-Z]+:$");
}
