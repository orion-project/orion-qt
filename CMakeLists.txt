set(ORI_NAME orion)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ORI_WITH_TESTS "Include testing framework and internal tests" OFF)

set(ORI_SOURCES
    core/OriArg.h
    core/OriFilter.h
    core/OriFloatingPoint.h
    core/OriResult.h
    core/OriTemplates.h
    core/OriVersion.h
    dialogs/OriBasicConfigDlg.h dialogs/OriBasicConfigDlg.cpp
    dialogs/OriConfigDlg.h dialogs/OriConfigDlg.cpp
    helpers/OriDialogs.h helpers/OriDialogs.cpp
    helpers/OriLayouts.h helpers/OriLayouts.cpp
    helpers/OriTheme.h helpers/OriTheme.cpp
    helpers/OriTools.h helpers/OriTools.cpp
    helpers/OriWidgets.h helpers/OriWidgets.cpp
    helpers/OriWindows.h helpers/OriWindows.cpp
    tools/OriDebug.h
    tools/OriHelpWindow.h tools/OriHelpWindow.cpp
    tools/OriHighlighter.h tools/OriHighlighter.cpp
    tools/OriLog.h tools/OriLog.cpp
    tools/OriLoremIpsum.h tools/OriLoremIpsum.cpp
    tools/OriMessageBus.h tools/OriMessageBus.cpp
    tools/OriMruList.h tools/OriMruList.cpp
    tools/OriPetname.h tools/OriPetname.cpp
    tools/OriSettings.h tools/OriSettings.cpp
    tools/OriStyler.h tools/OriStyler.cpp
    tools/OriTranslator.h tools/OriTranslator.cpp
    widgets/OriActions.h widgets/OriActions.cpp
    widgets/OriBackWidget.h widgets/OriBackWidget.cpp
    widgets/OriCharMap.h widgets/OriCharMap.cpp
    widgets/OriCodeEditor.h widgets/OriCodeEditor.cpp
    widgets/OriColorSelectors.h widgets/OriColorSelectors.cpp
    widgets/OriFlatToolBar.h
    widgets/OriFlowLayout.h widgets/OriFlowLayout.cpp
    widgets/OriInfoPanel.h widgets/OriInfoPanel.cpp
    widgets/OriLabels.h widgets/OriLabels.cpp
    widgets/OriLangsMenu.h widgets/OriLangsMenu.cpp
    widgets/OriMdiToolBar.h widgets/OriMdiToolBar.cpp
    widgets/OriMenuToolButton.h widgets/OriMenuToolButton.cpp
    widgets/OriMruMenu.h widgets/OriMruMenu.cpp
    widgets/OriOptionsGroup.h widgets/OriOptionsGroup.cpp
    widgets/OriPopupButton.h widgets/OriPopupButton.cpp
    widgets/OriPopupMessage.h widgets/OriPopupMessage.cpp
    widgets/OriSelectableTile.h widgets/OriSelectableTile.cpp
    widgets/OriStatusBar.h widgets/OriStatusBar.cpp
    widgets/OriStylesMenu.h widgets/OriStylesMenu.cpp
    widgets/OriTableWidgetBase.h widgets/OriTableWidgetBase.cpp
    widgets/OriValueEdit.h widgets/OriValueEdit.cpp
)

if(ORI_WITH_TESTS)
    list(APPEND ORI_SOURCES
        testing/OriTestBase.h testing/OriTestBase.cpp
        testing/OriTestManager.h
        testing/OriTestWindow.h testing/OriTestWindow.cpp
        testing/OriTestWindowTests.cpp
        testing/OriTimeMeter.h testing/OriTimeMeter.cpp
        tests/ori_test_Filter.cpp
        tests/ori_test_Math.cpp
        tests/ori_test_Templates.cpp
        tests/ori_test_Version.cpp
        tests/orion_tests.h
    )
endif()

add_library(${ORI_NAME} STATIC
    ${ORI_SOURCES}
)

target_link_libraries(${ORI_NAME} PRIVATE
    Qt::Widgets
)

target_compile_definitions(${ORI_NAME} PRIVATE
    QT_USE_QSTRINGBUILDER
)

target_include_directories(${ORI_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(${ORI_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
