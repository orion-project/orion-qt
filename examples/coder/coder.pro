QT += core gui widgets

CONFIG += c++17

TARGET = example_coder
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin   

include("../../orion.pri") 

SOURCES += \
    main.cpp

HEADERS +=
