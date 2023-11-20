#-------------------------------------------------
#
# Project created by QtCreator 2023-11-13T11:28:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


TARGET = example_colors
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")

SOURCES += \
    main.cpp

HEADERS +=

RESOURCES += \
    images.qrc
