QT += core gui widgets

CONFIG += c++17

TARGET = example_spellcheck
TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/../../bin

include("../../orion.pri")
include("../../orion_spellcheck.pri")

SOURCES += \
    main.cpp

HEADERS +=
