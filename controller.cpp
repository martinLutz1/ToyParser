#include "controller.h"
#include <QString>
#include <QDebug>

Controller::Controller() : reprType(0)
{
}

void Controller::connectMessagingSystemToView(MainWindow *view)
{
    QObject::connect(&parser, SIGNAL(sendMessage(Message&)), view, SLOT(notify(Message&)));
}


void Controller::notify(Message &msg)
{
    switch (msg.type) {
    case Message::view_parse_text:
    {
        QString* toyCodeSource = msg.data[0].stringPointer;
        parser.readProgramText(*toyCodeSource);
        parser.parseProgram();
        QString outputText = parser.getProgram(reprType);

        Message msg;
        msg.type = Message::parser_output;
        msg.data[0].stringPointer = &outputText;
        emit(sendMessage(msg));

        break;
    }
    case Message::view_representation_type_changed:
        reprType = msg.data[0].i;

    default:
        break;
    }
}

