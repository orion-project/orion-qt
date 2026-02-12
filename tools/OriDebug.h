#ifndef ORI_DEBUG_H
#define ORI_DEBUG_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

namespace Ori {
namespace Debug {

QString logsDir();

class ConsoleWindow : public QWidget
{
public:
    ConsoleWindow();
    void append(const QString& msg);
protected:
    bool event(QEvent *event) override;
private:
    QTextEdit *_log;
};

ConsoleWindow* consoleWindow();

void installMessageHandler(bool saveLogs = false);

} // namespace Debug
} // namespace Ori

#endif // ORI_DEBUG_H
