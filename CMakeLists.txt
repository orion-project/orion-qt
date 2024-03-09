set(ORI_NAME orion)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ORI_SOURCES
    dialogs/OriBasicConfigDlg.cpp
    dialogs/OriConfigDlg.cpp
    helpers/OriDialogs.cpp
    helpers/OriLayouts.cpp
    helpers/OriTheme.cpp
    helpers/OriTools.cpp
    helpers/OriWidgets.cpp
    helpers/OriWindows.cpp
    tools/OriHighlighter.cpp
    tools/OriLog.cpp
    tools/OriLoremIpsum.cpp
    tools/OriMessageBus.cpp
    tools/OriMruList.cpp
    tools/OriPetname.cpp
    tools/OriSettings.cpp
    tools/OriStyler.cpp
    tools/OriTranslator.cpp
    widgets/OriActions.cpp
    widgets/OriBackWidget.cpp
    widgets/OriCharMap.cpp
    widgets/OriCodeEditor.cpp
    widgets/OriColorSelectors.cpp
    widgets/OriFlowLayout.cpp
    widgets/OriInfoPanel.cpp
    widgets/OriLabels.cpp
    widgets/OriLangsMenu.cpp
    widgets/OriMdiToolBar.cpp
    widgets/OriMenuToolButton.cpp
    widgets/OriMruMenu.cpp
    widgets/OriOptionsGroup.cpp
    widgets/OriPopupButton.cpp
    widgets/OriPopupMessage.cpp
    widgets/OriSelectableTile.cpp
    widgets/OriStatusBar.cpp
    widgets/OriStylesMenu.cpp
    widgets/OriTableWidgetBase.cpp
    widgets/OriValueEdit.cpp
)

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
