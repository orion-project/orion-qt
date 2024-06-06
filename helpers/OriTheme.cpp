#include "OriTheme.h"

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

    QStringList lines = styleSheet.split('\n');
    for (int i = 0; i < lines.size(); i++) {
        int quotes = 0;
        const auto& line = lines.at(i);
        for (int j = 0; j < line.size()-1; j++) {
            if (line.at(j) == '"') {
                quotes++;
                continue;
            }
            if (line.at(j) == '/' && line.at(j+1) == '/') {
                if (quotes % 2 != 0) continue;
                lines[i] = line.left(j);
                break;
            }
        }
    }
    styleSheet = lines.join('\n');

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

    // Process platform-dependeent props
    auto options = QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption;
    static QRegularExpression propWin(QStringLiteral("^\\s*windows:(.*)$"), options);
    static QRegularExpression propLinux(QStringLiteral("^\\s*linux:(.*)$"), options);
    static QRegularExpression propMacos(QStringLiteral("^\\s*macos:(.*)$"), options);
    static QRegularExpression propQt5(QStringLiteral("^\\s*qt5:(.*)$"), options);
    static QRegularExpression propQt6(QStringLiteral("^\\s*qt6:(.*)$"), options);
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    styleSheet.replace(propQt6, QStringLiteral("\\1"));
    styleSheet.remove(propQt5);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    styleSheet.replace(propQt5, QStringLiteral("\\1"));
    styleSheet.remove(propQt6);
#endif
    return styleSheet;
}

#ifdef ORI_USE_STYLE_SHEETS
typedef QMap<Color, QColor> ColorMap;
Q_GLOBAL_STATIC(ColorMap, __colorMap);

QColor color(Color kind)
{
    return __colorMap->value(kind);
}

void setColors(const QMap<Color, QColor> &colors)
{
    for (auto it = colors.constBegin(); it != colors.constEnd(); it++)
        __colorMap->insert(it.key(), it.value());
}
#endif

} // namespace Theme
} // namespace Ori
