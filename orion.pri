INCLUDEPATH += $$PWD

DEFINES += QT_USE_QSTRINGBUILDER
macx: DEFINES += _LIBCPP_DISABLE_AVAILABILITY

# Include methods info in debug messages
greaterThan(QT_MAJOR_VERSION, 4) {
greaterThan(QT_MINOR_VERSION, 3) {
DEFINES -= QT_NO_MESSAGELOGCONTEXT
DEFINES += QT_MESSAGELOGCONTEXT
}}


# Enable C++11
greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG += c++11
}
else {
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_LFLAGS += -std=c++11
}

RESOURCES += \
    $$PWD/resources.qrc

HEADERS += \
    $$PWD/core/OriArg.h \
    $$PWD/core/OriResult.h \
    $$PWD/dialogs/OriConfigDlg.h \
    $$PWD/helpers/OriTheme.h \
    $$PWD/tools/OriHighlighter.h \
    $$PWD/tools/OriMessageBus.h \
    $$PWD/tools/OriPetname.h \
    $$PWD/widgets/OriActions.h \
    $$PWD/widgets/OriCodeEditor.h \
    $$PWD/widgets/OriColorSelectors.h \
    $$PWD/widgets/OriFlatToolBar.h \
    $$PWD/widgets/OriCharMap.h \
    $$PWD/widgets/OriInfoPanel.h \
    $$PWD/widgets/OriLabels.h \
    $$PWD/widgets/OriLangsMenu.h \
    $$PWD/widgets/OriMdiToolBar.h \
    $$PWD/widgets/OriMenuToolButton.h \
    $$PWD/widgets/OriMruMenu.h \
    $$PWD/widgets/OriOptionsGroup.h \
    $$PWD/widgets/OriPopupButton.h \
    $$PWD/widgets/OriPopupMessage.h \
    $$PWD/widgets/OriSelectableTile.h \
    $$PWD/widgets/OriStatusBar.h \
    $$PWD/widgets/OriStylesMenu.h \
    $$PWD/widgets/OriValueEdit.h \
    $$PWD/tools/OriDebug.h \
    $$PWD/tools/OriLoremIpsum.h \
    $$PWD/tools/OriSettings.h \
    $$PWD/tools/OriStyler.h \
    $$PWD/tools/OriTranslator.h \
    $$PWD/tools/OriWaitCursor.h \
    $$PWD/tools/OriMruList.h \
    $$PWD/tools/OriLog.h \
    $$PWD/helpers/OriWidgets.h \
    $$PWD/helpers/OriWindows.h \
    $$PWD/helpers/OriDialogs.h \
    $$PWD/core/OriFloatingPoint.h \
    $$PWD/core/OriTemplates.h \
    $$PWD/core/OriVersion.h \
    $$PWD/dialogs/OriBasicConfigDlg.h \
    $$PWD/helpers/OriTools.h \
    $$PWD/widgets/OriBackWidget.h \
    $$PWD/widgets/OriTableWidgetBase.h \
    $$PWD/widgets/OriFlowLayout.h \
    $$PWD/core/OriFilter.h \
    $$PWD/helpers/OriLayouts.h

SOURCES += \
    $$PWD/dialogs/OriConfigDlg.cpp \
    $$PWD/helpers/OriLayouts.cpp \
    $$PWD/helpers/OriTheme.cpp \
    $$PWD/tools/OriHighlighter.cpp \
    $$PWD/tools/OriMessageBus.cpp \
    $$PWD/tools/OriPetname.cpp \
    $$PWD/widgets/OriActions.cpp \
    $$PWD/widgets/OriCharMap.cpp \
    $$PWD/widgets/OriCodeEditor.cpp \
    $$PWD/widgets/OriColorSelectors.cpp \
    $$PWD/widgets/OriInfoPanel.cpp \
    $$PWD/widgets/OriLabels.cpp \
    $$PWD/widgets/OriLangsMenu.cpp \
    $$PWD/widgets/OriMdiToolBar.cpp \
    $$PWD/widgets/OriMenuToolButton.cpp \
    $$PWD/widgets/OriMruMenu.cpp \
    $$PWD/widgets/OriOptionsGroup.cpp \
    $$PWD/widgets/OriPopupButton.cpp \
    $$PWD/widgets/OriPopupMessage.cpp \
    $$PWD/widgets/OriSelectableTile.cpp \
    $$PWD/widgets/OriStatusBar.cpp \
    $$PWD/widgets/OriStylesMenu.cpp \
    $$PWD/widgets/OriValueEdit.cpp \
    $$PWD/tools/OriLoremIpsum.cpp \
    $$PWD/tools/OriSettings.cpp \
    $$PWD/tools/OriStyler.cpp \
    $$PWD/tools/OriTranslator.cpp \
    $$PWD/tools/OriMruList.cpp \
    $$PWD/tools/OriLog.cpp \
    $$PWD/helpers/OriWidgets.cpp \
    $$PWD/helpers/OriWindows.cpp \
    $$PWD/helpers/OriDialogs.cpp \
    $$PWD/dialogs/OriBasicConfigDlg.cpp \
    $$PWD/helpers/OriTools.cpp \
    $$PWD/widgets/OriBackWidget.cpp \
    $$PWD/widgets/OriTableWidgetBase.cpp \
    $$PWD/widgets/OriFlowLayout.cpp
