INCLUDEPATH += $$PWD

RESOURCES += \
    $$PWD/testing/test.qrc

HEADERS += \
    $$PWD/testing/OriTestBase.h \
    $$PWD/testing/OriTestManager.h \
    $$PWD/testing/OriTestWindow.h \
    $$PWD/testing/OriTimeMeter.h

SOURCES += \
    $$PWD/testing/OriTestBase.cpp \
    $$PWD/testing/OriTestWindow.cpp \
    $$PWD/testing/OriTestWindowTests.cpp \
    $$PWD/testing/OriTimeMeter.cpp
