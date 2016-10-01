#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T18:51:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example_tests
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")
include("../../orion_testing.pri")

SOURCES += main.cpp \
    sample_tests.cpp \
    sample_testgroup.cpp
