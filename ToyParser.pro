#-------------------------------------------------
#
# Project created by QtCreator 2016-08-18T16:12:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ToyParser
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    instructionrepresentation.cpp \
    parser.cpp \
    parserdata.cpp \
    parsererror.cpp \
    preparser.cpp \
    qstringoperations.cpp \
    message.cpp \
    application.cpp \
    controller.cpp

HEADERS  += mainwindow.h \
    instructionrepresentation.h \
    parser.h \
    parserdata.h \
    parsererror.h \
    preparser.h \
    qstringoperations.h \
    message.h \
    application.h \
    controller.h
FORMS    += mainwindow.ui

DISTFILES += \
    ToyParser.pro.user
