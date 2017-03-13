#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "message.h"

class MainWindow;
class Controller;

class Application
{
public:
    Application(int &argc, char *argv[]);
    ~Application();

    void init(); // Initialize the application
    int exec(); // Execute the application

private:
    QApplication app; // Dont derive from QApplication
    MainWindow* view; // Create view after QApplication
    Controller* controller;

};

#endif // APPLICATION_H
