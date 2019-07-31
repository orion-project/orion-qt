#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QPushButton>
#include <QTextBrowser>
#include <QTextEdit>

#include "helpers/OriWidgets.h"
#include "tools/OriSettings.h"

class MainWindow : public QSplitter
{
    Q_OBJECT

public:
    MainWindow()
    {
        Ori::Gui::setFontMonospace(&css);
        Ori::Gui::setFontMonospace(&html);
        css.setTabStopWidth(24);
        html.setTabStopWidth(24);
        css.setAcceptRichText(false);
        html.setAcceptRichText(false);

        setObjectName("mainWindow");
        setOrientation(Qt::Horizontal);

        addWidget(Ori::Gui::widgetV({
            Ori::Gui::layoutH({ new QLabel("<b>CSS</b>"), nullptr,
                Ori::Gui::button("Apply", this, SLOT(applyCss())) }), &css}));

        addWidget(Ori::Gui::splitterV(
            Ori::Gui::widgetV({ Ori::Gui::layoutH({ new QLabel("<b>HTML</d>"), nullptr,
                Ori::Gui::button("Apply", this, SLOT(applyHtml())) }), &html }),
            Ori::Gui::widgetV({ new QLabel("<b>Browse</b>"), &browser })
        ));

        setSizes({200, 500});

        Ori::Settings s;
        css.setPlainText(s.strValue("css"));
        html.setPlainText(s.strValue("html"));
        s.restoreWindowGeometry(this, QSize(700, 600));

        applyCss();
        applyHtml();
    }

    ~MainWindow()
    {
        Ori::Settings s;
        s.setValue("css", css.toPlainText());
        s.setValue("html", html.toPlainText());
        s.storeWindowGeometry(this);
    }

private slots:
    void applyCss()
    {
        browser.document()->setDefaultStyleSheet(css.toPlainText());
        browser.setHtml(html.toPlainText());
    }

    void applyHtml()
    {
        browser.setHtml(html.toPlainText());
    }

private:
    QTextEdit css, html;
    QTextBrowser browser;
};

#endif // MAINWINDOW_H
