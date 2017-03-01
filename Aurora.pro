#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T07:53:38
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++17
QMAKE_LFLAGS += -Wl,--large-address-aware

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Aurora
TEMPLATE = app


SOURCES += main.cpp\
        interface/mainwindow.cpp \
    interface/options.cpp \
    game.cpp \
    geometryprovider.cpp \
    modelcontainer.cpp \
    interface/hud.cpp \
    src/ray_intersect.cpp \
    src/fileops.cpp \
    src/neural.cpp \
    levels/arkanoid.cpp \
    src/mapgenerator.cpp

HEADERS  += interface/mainwindow.h \
    interface/options.h \
    game.h \
    geometryprovider.h \
    modelcontainer.h \
    interface/hud.h \
    include/ray_intersect.h \
    include/fileops.h \
    include/neural.h \
    levels/arkanoid.h \
    include/mapgenerator.h

FORMS    += interface/mainwindow.ui \
    interface/options.ui \
    interface/hud.ui

RESOURCES += \
    shaders.qrc \
    textures.qrc \
    icons.qrc
