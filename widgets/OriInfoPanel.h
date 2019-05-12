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
    explicit InfoPanel(QWidget *parent = nullptr);

    void setInfo(const QString& title, const QString& text);
    void setInfo(const QString& text);

    void setMargin(int value);
    void setSpacing(int value);

private:
    QLabel *_labelTitle, *_labelDescr;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_INFO_PANEL_H
