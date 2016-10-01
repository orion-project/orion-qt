#ifndef ORI_INFO_PANEL_H
#define ORI_INFO_PANEL_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {

class InfoPanel : public QFrame
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *parent = 0);

    void setInfo(const QString& text);

private:
    QLabel *labelDescr;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_INFO_PANEL_H
