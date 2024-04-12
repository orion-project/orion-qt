#ifndef ORI_STATUS_BAR_H
#define ORI_STATUS_BAR_H

#include <QStatusBar>

namespace Ori {
namespace Widgets {

class Label;

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    StatusBar(int count, QWidget *parent = nullptr);

    /** Connect handler to a section using signal-slot sintax.
     *
     * Example:
     *
     * _statusBar->connect(STATUS_LAMBDA, SIGNAL(doubleClicked()), _operations, SLOT(setupWavelength()));
     */
    void connect(int index, const char *signal, const QObject *receiver, const char *method);

    /** Connect handler to a section using lambda syntax.
     *
     * Example:
     *
     * _statusBar->connect(STATUS_FACTOR_X, &Ori::Widgets::Label::doubleClicked, [this]{ axisFactorDlgX(); });
     *
     * Each section of the status bar is Ori::Widgets::Label (because QLabel doesn't provide click signals)
     * so you have to include its header to make signals available:
     *
     * #include "widgets/OriLabels.h"
     */
    template <typename Func1, typename Func2>
    void connect(int index, Func1 func1, Func2 func2) {
        QStatusBar::connect(_sections[index], func1, func2); }

    void setText(int index, const QString& text);
    void setText(int index, const QString& text, const QString& tooltip);
    void setIcon(int index, const QString& path);
    void setHint(int index, const QString& hint) { setToolTip(index, hint); }
    void setToolTip(int index, const QString& tooltip);
    void setMargin(int index, int left, int right);
    void setVisible(int index, bool on);

    void clear(int index);
    void clear();

    void highlightError(int index);
    void highlightReset(int index);

    void setIconSize(const QSize& size);

    /// Adds a label with application version to the right side of the bar.
    Label* addVersionLabel();

    QPoint mapToGlobal(int index, const QPoint& p);

private:
    QVector<Label*> _sections;
    QSize _iconSize;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_STATUS_BAR_H
