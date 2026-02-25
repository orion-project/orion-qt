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
#include <QStack>
#include <QStatusBar>
#include <QTextBrowser>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>

#ifdef ORI_USE_ZIP_HELP
#include <zip.h>
#endif
#ifdef ORI_USE_MD4C_HELP
#include <md4c-html.h>
#endif

namespace {

Ori::HelpWindow* __instance = nullptr;

} // namespace

namespace Ori {

#ifdef ORI_USE_ZIP_HELP
struct ZipFile
{
    ZipFile(zip *z, const QString &fileName)
    {
        if (!z) {
            error = QString("Help file not found");
            qWarning() << error;
            return;
        }
    
        QString name = fileName.startsWith("./") ? fileName.mid(2) : fileName;
    
        struct zip_stat fi;
        zip_stat_init(&fi);
        auto fn = name.toStdString();
        if (zip_stat(z, fn.c_str(), ZIP_FL_UNCHANGED, &fi) < 0) {
            error = QString("Failed to get info for %1: %2").arg(name).arg(zip_error_strerror(zip_get_error(z)));
            return;
        }
        if (!(fi.valid & ZIP_STAT_SIZE)) {
            error = QString("Unable to get size of %1").arg(name);
        }
        zf = zip_fopen(z, fn.c_str(), ZIP_FL_UNCHANGED);
        if (!zf) {
            error = QString("Failed to open %1: %2").arg(name).arg(zip_error_strerror(zip_get_error(z)));
            return;
        }
        data = QByteArray(fi.size, 0);
        auto bytesRead = zip_fread(zf, data.data(), fi.size);
        if (bytesRead < 0) {
            error = QString("Failed to read %1: %2").arg(name).arg(zip_error_strerror(zip_file_get_error(zf)));
            return;
        }
        if (bytesRead != fi.size) {
            error = QString("Failed to read %1: expected %2 bytes but read %3 bytes").arg(name).arg(fi.size).arg(bytesRead);
            return;
        }
    }

    ~ZipFile()
    {
        if (zf)
            zip_fclose(zf);
    }
    
    bool ok() const { return error.isEmpty(); }
    
    QString error;
    QByteArray data;
    zip_file *zf = nullptr;
};
#endif

#ifdef ORI_USE_MD4C_HELP
struct Md2Html
{
    QString convert(const QByteArray &data)
    {
        _html.clear();
        int ret = md_html(data.constData(), data.size(), [](const MD_CHAR *data, MD_SIZE size, void *self){
            static_cast<Md2Html*>(self)->process(QString::fromUtf8(data, size));
        }, this, 0, 0);
        if (ret < 0)
            return "Failed to parse markdown";
        return _html.join("");
    }

private:
    void process(const QString &line)
    {
        //qDebug() << "parsed" << line;
        
        // Code blocks are parsed as 
        //    <pre><code
        //    class="language-      <-- these lines are optional 
        //    lua"                  <-- but closing slash and bracket
        //    \                     <-- goes on a separate call anyway
        //    >
        //    ...
        //    code line             <-- each code line doesn't have ending \n
        //    \n                    <-- they are passed on a separate call
        //    code line
        //    ...
        //    \n                    <-- this final line-break causes
        //                              issue because it's converted into 
        //                              additional <br> and looks like a
        //                              bottom margin inside the code block
        //    </code></pre>\n
        if (line.startsWith("<pre><code"))
        {
            _codeBlock = QStringList();
            // Qt can't draw borders and paddings around paragraphs and spans
            // but can do it for tables, so we imitate code block with a table
            // then the code block looks much nicer
            _html << QStringLiteral("<table class=\"code_block_table\" width=\"100%\">"
                "<tr><td class=\"code_block_cell\"><pre>");
        }
        else if (line.startsWith("</code></pre>"))
        {
            finishCodeBlock();
            _html << QStringLiteral("</pre></td></tr></table>");
        }
        else if (line == "<code>")
        {
            _inlineCode = true;
            _codeStarted = true;
            _codeBlock = QStringList();
            _html << QStringLiteral("<code class=\"inline_code\">");
        }
        else if (line == "</code>")
        {
            finishCodeBlock();
            _html << line;
        }
        else if (_codeBlock)
        {
            if (_codeStarted)
                _codeBlock->append(line);
            else if (line == ">")
                _codeStarted = true;
        }
        else if (line.startsWith("<a href=\""))
        {
            _linkStarted = true;
            _html << line;
        }
        else if (_linkStarted && line == "\">")
        {
            _linkStarted = false;
            if (_html.last().startsWith("http"))
                _html << QStringLiteral("\" class=\"external");
            _html << line;
        }
        else
            _html << line;
    }

    QStringList _html;
    std::optional<QStringList> _codeBlock;
    bool _inlineCode = false;
    bool _codeStarted = false;
    bool _linkStarted = false;
    
    void finishCodeBlock()
    {
        if (!_codeBlock->isEmpty())
        {
            if (_inlineCode)
            {
                // Qt can't make paddings in spans. But without them 
                // the gray background in inline code looks not nice.
                // So we emulate paddings with spaces
                _codeBlock->insert(0, QStringLiteral("&nbsp;"));
                _codeBlock->append(QStringLiteral("&nbsp;"));
            }
            else
            {
                if (_codeBlock->last().trimmed().isEmpty())
                    _codeBlock->removeLast();
            }
        }
        _html.append(*_codeBlock);
        _codeBlock = {};
        _codeStarted = false;
        _inlineCode = false;
    }
};
#endif

//------------------------------------------------------------------------------
//                               HelpBrowser
//------------------------------------------------------------------------------

class HelpBrowser : public QTextBrowser
{
public:
    HelpBrowser() : QTextBrowser()
    {
        setOpenExternalLinks(false);
        setOpenLinks(false);
        document()->setDocumentMargin(10);
    
        QFile f(":/style/help");
        if (!f.open(QIODevice::ReadOnly))
            qWarning() << "Unable to open resource file" << f.fileName() << f.errorString();
        else
            document()->setDefaultStyleSheet(QString::fromUtf8(f.readAll()));

        _helpDir = HelpWindow::getHelpDir();
        qDebug() << "Using help path" << _helpDir;

    #ifdef ORI_USE_ZIP_HELP
        if (!_helpDir.endsWith(".zip", Qt::CaseInsensitive))
        {
            qWarning() << "Given help path is not a zip-file, treating as directory" << _helpDir;
            setSearchPaths({ _helpDir });
            _useZipFile = false;
        }
        else
        {
            int errCode;
            auto helpFile = _helpDir.toStdString();
            _helpZip = zip_open(helpFile.c_str(), ZIP_RDONLY, &errCode);
            if (!_helpZip) {
                zip_error_t error;
                zip_error_init_with_code(&error, errCode);
                setPlainText(QString("Failed to open help file: %1").arg(zip_error_strerror(&error)));
                zip_error_fini(&error);
            }
        }
    #else
        setSearchPaths({ _helpDir });
    #endif

        connect(this, &QTextBrowser::anchorClicked, this, &HelpBrowser::loadUrl);
    }
    
    ~HelpBrowser()
    {
    #ifdef ORI_USE_ZIP_HELP
        if (_helpZip)
            zip_discard(_helpZip);
    #endif
    }

    QString addExt(const QString &name)
    {
        if (name.endsWith(".md", Qt::CaseInsensitive))
            return name;
        return name + ".md";
    }

    void load(const QString &name, bool addHistory = true)
    {
        //qDebug() << "load" << name;

        if (name.startsWith("http"))
        {
            QDesktopServices::openUrl(QUrl(name));
            return;
        }
    #ifdef ORI_USE_MD4C_HELP
        QString fileName = name;
        QString fragment;

        // Extract anchors from local paths
        if (int p = name.indexOf('#'); p > 0)
        {
            fileName = name.left(p);
            fragment = name.right(name.length()-p-1);
        }
        fileName = addExt(fileName);

        QByteArray fileData;
    #ifdef ORI_USE_ZIP_HELP
        if (_useZipFile)
        {
            ZipFile zf(_helpZip, fileName);
            if (!zf.ok())
            {
                setPlainText(QString("Failed to load document %1: %2").arg(name, zf.error));
                return;
            }
            fileData = zf.data;
        }
        else
    #endif
        {
            QString path = _helpDir + '/' + fileName;
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                setPlainText(QString("Failed to open markdown %1: %2").arg(name, file.errorString()));
                return;
            }
            fileData = file.readAll();
        }
        
        setHtml(Md2Html().convert(fileData));
        
        if (!fragment.isEmpty())
            scrollToAnchor(fragment);
        
        if (addHistory && !_currentPath.isEmpty())
        {
            _backHistory.push(_currentPath);
            _forthHistory.clear();
        }
        _currentPath = name;
    #else
        Q_UNUSED(addHistory);
        QUrl url;
        // Anchors are not parsed from local paths and browser complies
        // No document for file:./cam_settings_plot.md%23rescale-pixels
        if (int p = name.indexOf('#'); p > 0)
        {
            QString path = name.left(p);
            QString fragment = name.right(name.length()-p-1);
            url = QUrl::fromLocalFile(addExt(path));
            url.setFragment(fragment);
        }
        else url = QUrl::fromLocalFile(addExt(name));
        // TODO: browser does not navigate to the anchor created in markdown as
        // `### Rescale pixels <a id=rescale-pixels>&nbsp;</a>`
        // suggest a proper way to create anchors (another hack in updateHtml ?)
        // or do navigation manually (the `<a name=..` exists in the result html)
        setSource(url);
        updateHtml();
    #endif
    }

    void loadUrl(const QUrl &url)
    {
        load(url.toString());
    }
    
    void backward() override
    {
    #ifdef ORI_USE_MD4C_HELP
        if (_backHistory.isEmpty())
            return;
        _forthHistory.push(_currentPath);
        load(_backHistory.pop(), false);
    #else
        QTextBrowser::backward();
        updateHtml();
    #endif
    }

    void forward() override
    {
    #ifdef ORI_USE_MD4C_HELP
        if (_forthHistory.isEmpty())
            return;
        _backHistory.push(_currentPath);
        load(_forthHistory.pop(), false);
    #else
        QTextBrowser::forward();
        updateHtml();
    #endif
    }

    void reload() override
    {
    #ifdef ORI_USE_MD4C_HELP
        load(_currentPath, false);
    #else
        QTextBrowser::reload();
        updateHtml();
    #endif
    }
    
#ifdef ORI_USE_ZIP_HELP
    QVariant loadResource(int type, const QUrl &name) override
    {
        if (_useZipFile)
        {
            if (type == QTextDocument::ImageResource)
            {
                ZipFile zf(_helpZip, name.path());
                if (zf.ok())
                    return zf.data;
                return QVariant();
            }
        #ifndef ORI_USE_MD4C_HELP
            if (type == QTextDocument::MarkdownResource)
            {
                ZipFile zf(_helpZip, name.path());
                if (zf.ok())
                    return zf.data;
                return zf.error;
            }
        #endif
        }
        return QTextBrowser::loadResource(type, name);
    }
#endif
    
private:
#ifndef ORI_USE_MD4C_HELP
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
#endif
    
    QString _helpDir;
#ifdef ORI_USE_ZIP_HELP
    zip *_helpZip = nullptr;
    // Can be disabled in dev mode
    // if we passed a directory instead of zip-file path
    bool _useZipFile = true;
#endif
#ifdef ORI_USE_MD4C_HELP
    QString _currentPath;
    QStack<QString> _backHistory;
    QStack<QString> _forthHistory;
#endif
};

//------------------------------------------------------------------------------
//                               HelpWindow
//------------------------------------------------------------------------------

bool HelpWindow::isDevMode = false;

std::function<QString()> HelpWindow::getCssSrc = []{
    return QString(qApp->applicationDirPath() + "/../src/help.css");
};

std::function<QString()> HelpWindow::getHelpDir = []{
#ifdef ORI_USE_ZIP_HELP
    return QString(qApp->applicationDirPath() + "/help.zip");
#else
    return QString(qApp->applicationDirPath() + "/help");
#endif
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
    connect(_browser, QOverload<const QUrl&>::of(&QTextBrowser::highlighted), this, [statusBar](const QUrl& url){
        if (url.scheme().startsWith(QLatin1String("http")))
            statusBar->showMessage(url.toString());
        else statusBar->clearMessage();
    });

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
