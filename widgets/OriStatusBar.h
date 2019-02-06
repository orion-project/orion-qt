#ifndef ORI_STATUS_BAR_H
#define ORI_STATUS_BAR_H

#include <QStatusBar>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    StatusBar(int count, QWidget *parent = nullptr);

    void connect(int index, const char *signal, const QObject *receiver, const char *method);

    template <typename Func1, typename Func2>
    void connect(int index, Func1 func1, Func2 func2) {
        QStatusBar::connect(_sections[index], func1, func2); }

    void setText(int index, const QString& text);
    void setText(int index, const QString& text, const QString& tooltip);
    void setIcon(int index, const QString& path);
    void setHint(int index, const QString& hint) { setToolTip(index, hint); }
    void setToolTip(int index, const QString& tooltip);

    void clear(int index);
    void clear();

    void highlightError(int index);
    void highlightReset(int index);

    void setIconSize(const QSize& size);

    QPoint mapToGlobal(int index, const QPoint& p);

private:
    QVector<QLabel*> _sections;
    QSize _iconSize;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_STATUS_BAR_H
