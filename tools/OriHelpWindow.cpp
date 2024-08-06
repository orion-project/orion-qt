#include "OriHelpWindow.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "tools/OriHighlighter.h"
#include "tools/OriSettings.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QStatusBar>
#include <QTextBrowser>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>

namespace {

Ori::HelpWindow* __instance = nullptr;

} // namespace

namespace Ori {

//------------------------------------------------------------------------------
//                               HelpBrowser
//------------------------------------------------------------------------------

class HelpBrowser : public QTextBrowser
{
public:
    HelpBrowser() : QTextBrowser()
    {
        QFile f(":/style/help");
        if (!f.open(QIODevice::ReadOnly))
            qWarning() << "Unable to open resource file" << f.fileName() << f.errorString();
        document()->setDefaultStyleSheet(QString::fromUtf8(f.readAll()));
    }

    void load(const QString &name)
    {
        if (name.startsWith("http"))
        {
            QDesktopServices::openUrl(QUrl(name));
            return;
        }
        static QRegularExpression hasExt("^.+\\.{1}.+$");
        setSource(QUrl::fromLocalFile(
            hasExt.match(name).hasMatch() ? name : name + ".md"));
        updateHtml();
    }

    void loadUrl(const QUrl &url)
    {
        load(url.toString());
    }

    void backward() override
    {
        QTextBrowser::backward();
        updateHtml();
    }

    void forward() override
    {
        QTextBrowser::forward();
        updateHtml();
    }

    void reload() override
    {
        QTextBrowser::reload();
        updateHtml();
    }

private:
    void updateHtml()
    {
        // setMarkdown() ignores default stylesheet
        // As a workaround, we clean some markdown's styles we don't like
        // and re-set the same content as html, then the default stylesheet gets applied
        QString text = toHtml();
        static QVector<QPair<QRegularExpression, QString>> patterns = {
            { QRegularExpression("\\<h1.+?\\>"), "<h1>" },
            { QRegularExpression("\\<h2.+?\\>"), "<h2>" },
            { QRegularExpression("\\<h3.+?\\>"), "<h3>" },
            { QRegularExpression("\\<p.+?\\>"), "<p>" },
        };
        for (const auto& p : patterns)
            text.replace(p.first, p.second);
        setHtml(text);
    }
};

//------------------------------------------------------------------------------
//                               HelpWindow
//------------------------------------------------------------------------------

bool HelpWindow::isDevMode = false;

std::function<QString()> HelpWindow::getCssSrc = []{
    return QString(qApp->applicationDirPath() + "/../src/help.css");
};

std::function<QString()> HelpWindow::getHelpDir = []{
    return QString(qApp->applicationDirPath() + "/help");
};

void HelpWindow::showContent()
{
    openWindow();
    __instance->_browser->load("index.md");
}

void HelpWindow::showTopic(const QString& topic)
{
    openWindow();
    __instance->_browser->load(topic);
}

void HelpWindow::openWindow()
{
    if (!__instance)
        __instance = new HelpWindow();
    __instance->show();
    __instance->raise();
    __instance->activateWindow();
}

HelpWindow::HelpWindow() : QWidget()
{
#define T_ Ori::Gui::textToolButton

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/toolbar/help"));
    setWindowTitle(tr("%1 Manual").arg(qApp->applicationName()));
    setObjectName("HelpWindow");

    Ori::Settings s;

    auto statusBar = new QStatusBar;

    _browser = new HelpBrowser;
    _browser->setSearchPaths({ getHelpDir() });
    _browser->setOpenExternalLinks(false);
    _browser->setOpenLinks(false);
    connect(_browser, QOverload<const QUrl&>::of(&QTextBrowser::highlighted), this, [statusBar](const QUrl& url){
        if (url.scheme().startsWith(QLatin1String("http")))
            statusBar->showMessage(url.toString());
        else statusBar->clearMessage();
    });
    connect(_browser, &QTextBrowser::anchorClicked, _browser, &HelpBrowser::loadUrl);

    auto actnContent = new QAction(QIcon(":/ori_images/book"), tr("Content"), this);
    connect(actnContent, &QAction::triggered, this, &HelpWindow::showContent);

    auto actnBack = new QAction(QIcon(":/ori_images/navigate_back"), tr("Back"), this);
    auto actnForward = new QAction(QIcon(":/ori_images/navigate_fwd"), tr("Forward"), this);
    actnBack->setShortcut(QKeySequence::MoveToPreviousPage);
    actnForward->setShortcut(QKeySequence::MoveToNextPage);
    connect(_browser, &QTextBrowser::backwardAvailable, actnBack, &QAction::setEnabled);
    connect(_browser, &QTextBrowser::forwardAvailable, actnForward, &QAction::setEnabled);
    connect(actnBack, &QAction::triggered, _browser, &HelpBrowser::backward);
    connect(actnForward, &QAction::triggered, _browser, &HelpBrowser::forward);

    auto toolbar = new QToolBar;
    toolbar->addWidget(T_(actnContent));
    toolbar->addSeparator();
    toolbar->addWidget(T_(actnBack));
    toolbar->addWidget(T_(actnForward));

    if (isDevMode)
    {
        toolbar->addSeparator();
        toolbar->addAction(QIcon(":/ori_images/update"), tr("Refresh"), _browser, &HelpBrowser::reload);
        toolbar->addAction(QIcon(":/ori_images/protocol"), tr("Edit Stylesheet"), this, &HelpWindow::editStyleSheet);
    }

    Ori::Layouts::LayoutV({toolbar, _browser, statusBar})
        .setSpacing(0).setMargins(3, 0, 3, 0).useFor(this);

    s.restoreWindowGeometry(this, {800, 600});

#undef T_
}

HelpWindow::~HelpWindow()
{
    Ori::Settings s;
    s.storeWindowGeometry(this);

    __instance = nullptr;
}

void HelpWindow::editStyleSheet()
{
    QString styleFile = getCssSrc();

    auto editor = new QPlainTextEdit;
    Ori::Gui::setFontMonospace(editor);
    editor->setPlainText(_browser->document()->defaultStyleSheet());
    Ori::Highlighter::setHighlighter(editor, ":/syntax/css");

    auto html = new QPlainTextEdit;
    Ori::Gui::setFontMonospace(html);
    html->setPlainText(_browser->toHtml());

    auto tabs = new QTabWidget;
    tabs->addTab(editor, "CSS");
    tabs->addTab(html, "HTML");

    auto applyButton = new QPushButton("Apply");
    connect(applyButton, &QPushButton::clicked, this, [this, editor]{
        _browser->document()->setDefaultStyleSheet(editor->toPlainText());
    });

    auto reloadButton = new QPushButton("Reload");
    connect(reloadButton, &QPushButton::clicked, this, [this, editor, html]{
        editor->setPlainText(_browser->document()->defaultStyleSheet());
        html->setPlainText(_browser->toHtml());
    });

    auto saveButton = new QPushButton("Save");
    saveButton->connect(saveButton, &QPushButton::clicked, editor, [styleFile, editor]{
        QFile f(styleFile);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qWarning() << "Unable to open file for writing" << styleFile << f.errorString();
            return;
        }
        f.write(editor->toPlainText().toUtf8());
        qDebug() << "Saved" << styleFile;
    });

    auto wnd = Ori::Layouts::LayoutV({
        new QLabel(styleFile),
        tabs,
        Ori::Layouts::LayoutH({
            Ori::Layouts::Stretch(),
            reloadButton,
            applyButton,
            saveButton,
        }).setMargin(6)
    }).setMargin(3).setSpacing(6).makeWidget();
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->setWindowTitle("Stylesheet Editor");
    wnd->setWindowIcon(QIcon(":/ori_images/protocol"));
    wnd->resize(300, 600);
    wnd->show();
}

} // namespace Ori
