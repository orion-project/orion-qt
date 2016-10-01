#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T19:49:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = unit_tests
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")
include("../../orion_testing.pri")
include("../../orion_tests.pri")

SOURCES += \
    main.cpp

