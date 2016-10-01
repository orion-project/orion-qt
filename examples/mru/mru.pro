#-------------------------------------------------
#
# Project created by QtCreator 2016-05-07T20:37:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example_mru
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")

SOURCES += main.cpp

HEADERS += MainWindow.h

