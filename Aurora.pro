#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T07:53:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Aurora
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    options.cpp \
    game.cpp \
    glsphere.cpp

HEADERS  += mainwindow.h \
    options.h \
    game.h \
    glsphere.h

FORMS    += mainwindow.ui \
    options.ui

DISTFILES += \
    fragmentShader.frag \
    vertexShader.vert
