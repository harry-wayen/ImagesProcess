#-------------------------------------------------
#
# Project created by QtCreator 2017-06-16T17:34:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = image
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    midfilter.cpp \
    blockcode.cpp \
    threadhold.cpp

HEADERS  += mainwindow.h \
    midfilter.h \
    blockcode.h \
    threadhold.h

FORMS    += mainwindow.ui \
    midfilter.ui \
    blockcode.ui \
    threadhold.ui

RESOURCES += \
    images.qrc
