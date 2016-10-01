#-------------------------------------------------
#
# Project created by QtCreator 2015-10-14T21:11:40
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = color_mixer
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")

HEADERS += \
    ColorPreview.h \
    ColorSelector.h \
    MainWindow.h \
    PaletteColorSelector.h \
    PredefinedColorSelector.h \
    SelectColorButton.h

SOURCES += \
    ColorPreview.cpp \
    ColorSelector.cpp \
    main.cpp \
    MainWindow.cpp \
    PaletteColorSelector.cpp \
    PredefinedColorSelector.cpp \
    SelectColorButton.cpp

