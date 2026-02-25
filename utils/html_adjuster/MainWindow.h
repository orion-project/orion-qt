#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QTabWidget>

#include "helpers/OriWidgets.h"
#include "helpers/OriLayouts.h"
#include "tools/OriSettings.h"

using namespace Ori::Gui;
using namespace Ori::Layouts;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow()
    {
        setObjectName("mainWindow");
        
        tabsCss->addTab(css, "CSS");
        tabsCss->setCornerWidget(button("Apply", this, &MainWindow::applyCss));

        tabsCode->addTab(html, "HTML");
        tabsCode->addTab(md, "Markdown");
        tabsCode->setCornerWidget(button("Apply", this, &MainWindow::applyCode));
        
        tabsHtml->addTab(browser, "Browse");
        tabsHtml->addTab(html1, "HTML");

        LayoutH({
            splitterH(tabsCss, splitterV(tabsCode, tabsHtml), 200, 500)
        }).setMargin(5).useFor(this);

        Ori::Settings s;
        css->setPlainText(s.strValue("css"));
        html->setPlainText(s.strValue("html"));
        md->setPlainText(s.strValue("md"));
        s.restoreWindowGeometry(this, QSize(700, 600));

        applyCss();
    }

    ~MainWindow()
    {
        Ori::Settings s;
        s.setValue("css", css->toPlainText());
        s.setValue("html", html->toPlainText());
        s.setValue("md", md->toPlainText());
        s.storeWindowGeometry(this);
    }
    
    void applyCss()
    {
        browser->document()->setDefaultStyleSheet(css->toPlainText());
        applyCode();
    }

    void applyCode()
    {
        if (tabsCode->currentIndex() == 0)
            browser->setHtml(html->toPlainText());
        else
            browser->setMarkdown(md->toPlainText());
        html1->setPlainText(browser->toHtml());
        tabsHtml->setCurrentIndex(0);
    }
    
    QPlainTextEdit* makeEditor()
    {
        auto ed = new QPlainTextEdit;
        Ori::Gui::setFontMonospace(ed);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        ed->setTabStopDistance(24);
    #else
        ed->setTabStopWidth(24);
    #endif
        return ed;
    }
    
    QTabWidget* makeTabs()
    {
        auto tabs = new QTabWidget;
        tabs->setStyleSheet("QTabWidget{border:none}");
        return tabs;
    }
    
    QTabWidget *tabsCss = makeTabs();
    QTabWidget *tabsCode = makeTabs();
    QTabWidget *tabsHtml = makeTabs();
    QPlainTextEdit *css = makeEditor();
    QPlainTextEdit *html = makeEditor();
    QPlainTextEdit *md = makeEditor();
    QPlainTextEdit *html1 = makeEditor();
    QTextBrowser *browser = new QTextBrowser;
};

#endif // MAINWINDOW_H
