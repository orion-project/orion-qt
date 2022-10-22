#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "tools/OriHighlighter.h"
#include "tools/OriSettings.h"
#include "widgets/OriCodeEditor.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>

using namespace Ori::Gui;
using namespace Ori::Highlighter;
using namespace Ori::Layouts;
using namespace Ori::Widgets;

#define FILE_FILTERS "Highlighter file (*.ohl);;All files (*.*)"

class MainWindow : public QSplitter
{
public:
    CodeEditor *editorCode = new CodeEditor;
    QPlainTextEdit *editorSample = new QPlainTextEdit;
    QListWidget *logView = new QListWidget;
    QSharedPointer<Spec> spec = QSharedPointer<Spec>(new Spec);
    Highlighter *highlighter = new Highlighter(editorSample, spec);
    QString lastDir;
    QLabel *labelFile = new QLabel("(file not selected)");

    MainWindow()
    {
        setWindowTitle("Highlighter Spec Editor");
        setObjectName("mainWindow");
        setOrientation(Qt::Vertical);
        setContentsMargins(3, 3, 3, 3);

        setFontMonospace(editorCode);
        setHighlighter(editorCode, qApp->applicationDirPath() + "/../syntax/ohl.ohl");

        setFontMonospace(editorSample);

        setFontMonospace(logView);

        auto butOpen = new QPushButton("Open");
        connect(butOpen, &QPushButton::clicked, this, &MainWindow::openSpec);
        auto butSave = new QPushButton("Save");
        connect(butSave, &QPushButton::clicked, this, [this]{ saveSpec(false); });
        auto butSaveAs = new QPushButton("Save As");
        connect(butSaveAs, &QPushButton::clicked, this, [this]{ saveSpec(true); });
        auto butApply = new QPushButton("Parse and Highlight");
        connect(butApply, &QPushButton::clicked, this, [this]{ applySpec(false); });

        auto content = splitterH(
                      LayoutV({
                                  LayoutH({
                                      new QLabel("<b>Highlighter Code</b>"),
                                      Stretch(),
                                      butOpen,
                                      butSave,
                                      butSaveAs,
                                      Space(12),
                                  }),
                                  labelFile,
                                  editorCode
                              }).setMargin(0).makeWidget(),
                      LayoutV({
                                  LayoutH({
                                      new QLabel("<b>Sample Text</b>"),
                                      Stretch(),
                                      butApply,
                                  }),
                                  new QLabel("This text will also be stored to highlighter spec file"),
                                  editorSample
                              }).setMargin(0).makeWidget());

        addWidget(content);
        addWidget(logView);

        Ori::Settings s;
        lastDir = s.value("lastDir").toString();
        int sampleW = s.value("sampleW").toInt();
        int logH = s.value("logH", 100).toInt();
        s.restoreWindowGeometry(this);

        // Splitter sizes can be adjusted only after window has been shown
        QTimer::singleShot(100, this, [this, content, sampleW, logH]{
            if (sampleW > 0) {
                auto sz = content->sizes();
                content->setSizes({ sz.at(0)+sz.at(1)-sampleW, sampleW });
            }
            if (logH > 0) {
                auto sz = sizes();
                setSizes({ sz.at(0)+sz.at(1)-logH, logH });
            }
        });
    }

    ~MainWindow()
    {
        Ori::Settings s;
        s.setValue("sampleW", editorSample->width());
        s.setValue("logH", logView->height());
        s.setValue("lastDir", lastDir);
        s.storeWindowGeometry(this);
    }

    void openSpec()
    {
        QString fn = QFileDialog::getOpenFileName(this, "", lastDir, FILE_FILTERS);
        if (fn.isEmpty()) return;

        QFileInfo fi(fn);
        lastDir = fi.absoluteDir().path();

        auto res = createSpecFromFile(fn, true);
        if (!res.error.isEmpty())
            return Ori::Dlg::error("Unable to load highlighter: " + res.error);

        spec = res.spec;
        editorCode->setPlainText(spec->rawCode());
        editorSample->setPlainText(spec->rawSample());
        logView->addItem("File: " + spec->meta.source);
        logView->addItem("Name: " + spec->meta.name);
        logView->addItem("Title: " + spec->meta.title);

        showWarns(res.warns);

        if (highlighter) delete highlighter;
        highlighter = new Highlighter(editorSample, spec);

        labelFile->setText(fn);
    }

    bool applySpec(bool withRawData)
    {
        auto code = editorCode->toPlainText();
        auto warns = loadSpec(spec, &code, withRawData);
        showWarns(warns);
        highlighter->rehighlight();
        return warns.isEmpty();
    }

    void saveSpec(bool selectFn)
    {
        bool ok = applySpec(true);
        if (!ok && !Ori::Dlg::yes("There are parse warnings, save anyway?"))
            return;

        QString fn = spec->meta.source;
        if (selectFn || fn.isEmpty())
            fn = QFileDialog::getSaveFileName(this, "", lastDir, FILE_FILTERS);

        if (fn.isEmpty())
            return;

        if (!selectFn && !Ori::Dlg::yes("Overwrite highlighter file?\n\n" + fn))
            return;

        // Code already copied into spec because we applied it withRawData
        spec->setRawSample(editorSample->toPlainText());

        logView->addItem("Saving: " + fn);

        QFile file(fn);
        if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate) ||
                file.write(spec->storableString().toUtf8()) < 0)
        {
            QString err = "Failed to write file: " + file.errorString();
            logView->addItem(err);
            return Ori::Dlg::error(err);
        }

        spec->meta.source = fn;
        logView->addItem("Saved: " + fn);
        labelFile->setText(fn);
    }

    void showWarns(const SpecWarnings& warns)
    {
        editorCode->setLineHints(warns);

        auto it = warns.constBegin();
        while (it != warns.constEnd()) {
            auto lineNo = it.key();
            auto text = it.value();
            if (lineNo > 0)
                text = QString("Line %1: %2").arg(lineNo).arg(text);
            logView->addItem(text);
            it++;
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");

    // It is used as settings storage location.
    a.setOrganizationName("orion-project.org");

    MainWindow w;
    w.show();

    return a.exec();
}
