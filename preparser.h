#ifndef PREPARSER_H
#define PREPARSER_H

#include "parserdata.h"
#include "qstringoperations.h"

class PreParser
{
public:
    PreParser(QStringOperations *qStringOps, ParserData *parserData);
    void preParseProgram();
    void resetPreParserStates();

private:
    QStringOperations *qStringOps;
    ParserData *parserData;

    short amountOfVariables; // Used for variable-Arrays lengths
    short amountOfLabels; // Used for lable-arrays lengths
    short amountSaveDataInstructions; // How many saveData Instructions? Used for saveData-Arrays lengths
    short saveDataLineNumber[RAMSIZE];

    QString variableName[RAMSIZE];
    QString labelName[RAMSIZE];
    short saveDataAdress[RAMSIZE];

    short variableValue[RAMSIZE];
    short labelPosition[RAMSIZE];
    short saveDataValue[RAMSIZE];


    // Returns the amount of instructions in "wholeProgram"
    short countAmountInstructions();

    // Writes the values after ":" into saveDataAdress and saveDataValue. Then increments amountSaveDataInstructions
    void checkProgramForSaveDataInstruction();

    // Check addresses for right range and write them to ram, if everything fine
    void writeSaveDataInstructions(int amountAssignedInstructions);

    // Checks for "var=$value", saves them in the variableName and variableValue-arrays and removes them from "wholeProgram"
    void checkProgramForVariableDeclarations();

    // Looks for variables, that are known due to checkProgramForVariableDeclarations() and replaces them by its value
    void replaceVariablesWithValues(int amountInstructions);

    // Checks for "label:", saves them in the labelName and labelPosition-arrays and removes them from "wholeProgram"
    void checkProgramForLabelDeclarations();

    // Looks for labels, that are known due to checkProgramForLabelDeclarations() and replaces the by its value
    void replaceLabelsWithValues();
};

#endif // PREPARSER_H
