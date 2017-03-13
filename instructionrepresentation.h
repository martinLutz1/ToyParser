#ifndef INSTRUCTIONREPRESENTATION_H
#define INSTRUCTIONREPRESENTATION_H

#define MAXRANGE 65536
#include "QString"

class InstructionRepresentation
{
public:
    InstructionRepresentation();
    QString asBinary(unsigned short instruction);
    QString asHex(unsigned short instruction);
    QString asMnemonic(unsigned short instruction, int numberType = 10);
    QStringList asMnemonicSplitted(unsigned short instruction, int numberType = 10);


private:
};

#endif // INSTRUCTIONREPRESENTATION_H
