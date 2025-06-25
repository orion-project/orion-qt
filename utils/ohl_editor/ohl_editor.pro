#-------------------------------------------------
#
# Project created by QtCreator 2022-10-22T08:33:40
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

win32-msvc* {
    QMAKE_CXXFLAGS += /std:c++20
} 

TARGET = ohl_editor
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")

HEADERS +=

SOURCES += \
    main.cpp
