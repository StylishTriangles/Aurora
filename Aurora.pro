#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T07:53:38
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Aurora
TEMPLATE = app


SOURCES += main.cpp\
        interface/mainwindow.cpp \
    interface/options.cpp \
    game.cpp \
    geometryprovider.cpp \
    modelcontainer.cpp

HEADERS  += interface/mainwindow.h \
    interface/options.h \
    game.h \
    geometryprovider.h \
    modelcontainer.h

FORMS    += interface/mainwindow.ui \
    interface/options.ui

RESOURCES += \
    shaders.qrc \
    textures.qrc
