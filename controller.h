#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "parser.h"
#include "message.h"
#include "mainwindow.h"


class Controller : public QObject
{
    Q_OBJECT
public:
    Controller();
    void connectMessagingSystemToView(MainWindow* view);

private:
    Parser parser;
    int reprType;

public slots:
    void notify(Message &msg);

signals:
    void sendMessage(Message &msg);
};

#endif // CONTROLLER_H
