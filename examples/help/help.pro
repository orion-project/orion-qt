#-------------------------------------------------
#
# Project created by QtCreator 2024-08-26T12:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example_help
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")

RESOURCES += app.qrc

SOURCES += main.cpp
