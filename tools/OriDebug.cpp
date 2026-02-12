#include "OriDebug.h"

#include <QApplication>
#include <QDateTime>
#include <QEvent>
#include <QFileInfo>
#include <QMenu>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
#include <QPointer>
#include <QVBoxLayout>

namespace Ori {
namespace Debug {

class LogMsgEvent : public QEvent
{
public:
    LogMsgEvent(const QString& text) : QEvent(QEvent::User), text(text) {}
    QString text;
};

//------------------------------------------------------------------------------
//                             ConsoleWindow
//------------------------------------------------------------------------------

static bool __firstTime = true;
static QPointer<ConsoleWindow> __console;
Q_GLOBAL_STATIC(QStringList, __logMessages);

ConsoleWindow* consoleWindow(bool show)
{
    if (!__console)
    {
        __console = new ConsoleWindow;
        __console->resize(800, 300);
        
        for (const auto &msg : *__logMessages)
            __console->append(msg);
        __console->scrollToEnd();
    }
    if (show)
    {
        __console->show();
        __console->activateWindow();
    }
    return __console;
}

ConsoleWindow::ConsoleWindow() : QWidget()
{
    // setWindowFlag(Qt::Tool, true);
    // setWindowFlag(Qt::WindowStaysOnTopHint, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle(qApp->applicationName() + " Log");

    _log = new QTextEdit;
    _log->setReadOnly(true);
#ifdef Q_OS_WIN
    _log->setFont(QFont("Courier", 9));
#else
    _log->setFont(QFont("Monospace", 10));
#endif
    _log->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_log, &QTextEdit::customContextMenuRequested, this, &ConsoleWindow::showContextMenu);
    
    _actnClearLog = new QAction(tr("Clear Log"), this);
    connect(_actnClearLog, &QAction::triggered, this, &ConsoleWindow::clearLog);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->addWidget(_log);
}

void ConsoleWindow::append(const QString& msg)
{
    qApp->sendEvent(this, new LogMsgEvent(msg));
}

bool ConsoleWindow::event(QEvent *event)
{
    if (auto e = dynamic_cast<LogMsgEvent*>(event); e) {
        _log->append(e->text);
        return true;
    }
    return QWidget::event(event);
}

void ConsoleWindow::scrollToEnd()
{
    auto cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    _log->setTextCursor(cursor);
    _log->ensureCursorVisible(); 
}

void ConsoleWindow::showContextMenu(const QPoint &pos)
{
    auto menu = _log->createStandardContextMenu(pos);
    menu->addSeparator(); 
    menu->addAction(_actnClearLog);
    menu->exec(_log->viewport()->mapToGlobal(pos));
    delete menu; 
}

void ConsoleWindow::clearLog()
{
    _log->clear();
    __logMessages->clear();
}

//------------------------------------------------------------------------------
//                                   LogFile
//------------------------------------------------------------------------------

QString logsDir()
{
    return qApp->applicationDirPath();
}

struct LogFile
{
    LogFile()
    {
        path = logsDir()
            % '/'
            % QFileInfo(qApp->applicationFilePath()).baseName().toLower()
            % '-'
            % QDateTime::currentDateTime().toString("yyyy-MM-ddTHH-mm-ss")
            % ".log";
    }

    QString path;
};

bool __saveLogs = false;
Q_GLOBAL_STATIC(LogFile, __logFile);

//------------------------------------------------------------------------------
//                              installMessageHandler
//------------------------------------------------------------------------------


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QtMessageHandler __defaultHandler = nullptr;
#else
QtMsgHandler __defaultHandler = nullptr;
#endif

QString messageTypeName(QtMsgType type)
{
    switch (type)
    {
    case QtDebugMsg: return "DEBUG";
    case QtWarningMsg: return "WARNING";
    case QtCriticalMsg: return "CRITICAL";
    case QtFatalMsg: return "FATAL";
    default: return QString();
    }
}

QString sanitizeHtml(const QString& msg)
{
    return QString(msg).replace("<", "&lt;").replace(">", "&gt;").replace("\n", "<br>");
}

bool mayInoreMessage(const QString& message)
{
    static QStringList ignoredMessages({
        // message sometimes appears when switching between windows on Xfce
        QString::fromLatin1("QXcbWindow: Unhandled client message: \"_GTK_LOAD_ICONTHEMES\""),
    });

    return ignoredMessages.contains(message);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    if (__defaultHandler)
        (*__defaultHandler)(type, context, message);

#ifdef Q_OS_LINUX
    if (mayInoreMessage(message)) return;
#endif

    if (__saveLogs)
    {
        QString msg = qFormatLogMessage(type, context, message);
        // Note that the C++ standard guarantees that static FILE *f is initialized in a thread-safe way.
        // We can also expect fprintf() and fflush() to be thread-safe, so no further synchronization is necessary
        static FILE *f = fopen(qPrintable(__logFile->path), "a");
        fprintf(f, "%s\n", qPrintable(msg));
        fflush(f);
    }

    QString msg = QString("<p><b>%1</b>: %2").arg(messageTypeName(type), sanitizeHtml(message));

    // Messages inside of Qt-code has no context filled
    // but function info already built into the message text
    if (context.file)
        msg += QString("<br><font color=gray>(%1:%2, %3</font>")
            .arg(context.file).arg(context.line).arg(context.function);

    __logMessages->append(msg);
    if (__console)
    {
        __console->append(msg);
    }
    else if (__firstTime)
    {
        __firstTime = false;

        // Show console window on first message after application starts
        // to make it clear that it's enabled.
        // Then, if user closed the window,
        // it will non popup again automaticallly
        // because it's quite annoying

    #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        if (!qApp->instance()->thread()->isCurrentThread())
    #else
        if (qApp->instance()->thread() != QThread::currentThread())
    #endif
        {
            // UI objects must be created only in the main thread
            QMetaObject::invokeMethod(qApp, []{ consoleWindow(false); });
        }
        else consoleWindow(false);
        
        // First time the window often could be shown together with (and hiddden by) the main window
        // Wait small time to be sure that all other windows are activated and show console on the top
        QTimer::singleShot(200, __console, []{
            __console->show();
            __console->activateWindow();
        });
    }
}
#else
void messageHandler(QtMsgType type, const char* msg)
{
    if (__defaultHandler)
        (*__defaultHandler)(type, msg);

#ifdef Q_OS_LINUX
    if (mayInoreMessage(message)) return;
#endif

    QString message = QString::fromUtf8(msg);
    consoleWindow()->append(QString("<p><b>%1</b>: %2<br>").arg(messageType(type)).arg(sanitizeHtml(message)));
}
#endif

void installMessageHandler(bool saveLogs)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    __saveLogs = saveLogs;

    if (__saveLogs) {
        if (qEnvironmentVariable("QT_MESSAGE_PATTERN").isEmpty())
            qSetMessagePattern("%{time yyyy-MM-ddTHH:mm:ss.zzz} [%{type}] %{message}");
    }

    __defaultHandler = qInstallMessageHandler(messageHandler);
#else
    __defaultHandler = qInstallMsgHandler(messageHandler);
#endif
}

} // namespace Debug
} // namespace Ori
