#-------------------------------------------------
#
# Project created by QtCreator 2016-08-12T16:04:00
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serverthd.cpp \
    transmitthd.cpp \
    recvthd.cpp

HEADERS  += mainwindow.h \
    serverthd.h \
    transmitthd.h \
    recvthd.h

FORMS    += mainwindow.ui

DISTFILES += \
    README
