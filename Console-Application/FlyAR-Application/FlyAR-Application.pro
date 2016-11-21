#-------------------------------------------------
#
# Project created by QtCreator 2016-11-13T19:13:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlyAR-Application
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    flyar.cpp \
    tablewindow.cpp \
    objtable.cpp

HEADERS  += mainwindow.h \
    flyar.h \
    tablewindow.h \
    objtable.h

FORMS    += mainwindow.ui
