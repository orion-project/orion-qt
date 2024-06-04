#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>

namespace Ori {
namespace Theme {

QString resourceName()
{
    return QStringLiteral(":/style/app");
}

QString rawFileName()
{
    // TODO: adjust for macOS
    return qApp->applicationDirPath() + QStringLiteral("/../src/app.qss");
}

QString loadRawStyleSheet()
{
    QFile f(resourceName());
    bool ok = f.open(QIODevice::ReadOnly);
    if (!ok)
    {
        qWarning() << "Unable to open resource file" << f.fileName() << f.errorString();
        return QString();
    }
    return QString::fromUtf8(f.readAll());
}

QString saveRawStyleSheet(const QString& text)
{
    QFile f(rawFileName());
    if (!f.exists())
        return QString("File doesn't exist: %1").arg(f.fileName());
    if (!f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        return QString("Failed to open \"%1\" for writing: %2").arg(f.fileName(), f.errorString());
    if (f.write(text.toUtf8()) < 0)
        return QString("Failed to write file \"%1\": %2").arg(f.fileName(), f.errorString());
    return QString();
}

QString makeStyleSheet(const QString& rawStyleSheet)
{
    QString styleSheet = rawStyleSheet;

    // Interpolate vars
    static QRegularExpression varExpr(QStringLiteral("(\\$[a-zA-Z_][a-zA-Z_-]*)\\s*:\\s*(.+);"));
    auto m = varExpr.match(styleSheet);
    while (m.hasMatch())
    {
        QString varName = m.captured(1);
        QString varValue = m.captured(2);
        styleSheet.remove(m.capturedStart(), m.capturedLength());
        styleSheet.replace(varName, varValue);

        m = varExpr.match(styleSheet);
    }

    static QRegularExpression commentExpr(QStringLiteral("^.*\\/\\/.*$"));
    m = commentExpr.match(styleSheet);
    while (m.hasMatch())
    {
        styleSheet.remove(m.capturedStart(), m.capturedLength());
        m = commentExpr.match(styleSheet, m.capturedEnd());
    }

    // Process platform-dependeent props
    auto options = QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption;
    static QRegularExpression propWin(QStringLiteral("^\\s*windows:(.*)$"), options);
    static QRegularExpression propLinux(QStringLiteral("^\\s*linux:(.*)$"), options);
    static QRegularExpression propMacos(QStringLiteral("^\\s*macos:(.*)$"), options);
#if defined(Q_OS_WIN)
    styleSheet.replace(propWin, QStringLiteral("\\1"));
    styleSheet.remove(propLinux);
    styleSheet.remove(propMacos);
#elif defined(Q_OS_LINUX)
    styleSheet.replace(propLinux, QStringLiteral("\\1"));
    styleSheet.remove(propWin);
    styleSheet.remove(propMacos);
#elif defined(Q_OS_MAC)
    styleSheet.replace(propMacos, QStringLiteral("\\1"));
    styleSheet.remove(propWin);
    styleSheet.remove(propLinux);
#endif

    return styleSheet;
}

} // namespace Theme
} // namespace Ori
