#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include "qstringoperations.h"
#include "parserdata.h"
#include "preparser.h"
#include "instructionrepresentation.h"
#include "message.h"


class Parser : public QObject
{
    Q_OBJECT
public:
    Parser();

    enum reprType
    {
        binary,
        decimal,
        hexadecimal,
        mnemonic
    };

    // Reads program form text and writes it to wholeProgram
    bool readProgramText(QString toyCode);

    // Reads program from file and writes it to wholeProgram
    bool readProgramFile(QString filename);

    // Parses the program.toy into the internal parser-RAM (parserData.parserRAM)
    bool parseProgram();

    QString getProgram(int type);

    void resetParserStates();

    // Converts "sourceType"-string to short. Accepts only simplified strings (use simplifyString())
    short getInstruction(int sourceType, QRegularExpression source, QString line, int lineNumber);

private:
    QStringOperations qStringOps;
    InstructionRepresentation instrRepr;
    ParserData parserData;
    PreParser preParser;

    // Returns BINARY(2), HEXADECIMAL(16), MNEMONIC(3)
    // Is more or less an assumption, can still contain errors (just to classify the instruction)
    int checkInputType(QString line, int lineNumber);

    short getInstructionFromMnemonic(QString line, int lineNumber);

    void checkForInfinityLoop();

    // Saves working RAM-state
    void backupRAM();

    // Loads last working RAM-state
    void loadRAMBackup();

signals:
    void sendMessage(Message &msg);
};

#endif // PARSER_H
