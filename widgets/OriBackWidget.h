#ifndef ORIBACKWIDGET
#define ORIBACKWIDGET

#include <QWidget>

namespace Ori {
namespace Widgets {

class BackWidget : public QWidget
{
public:
    explicit BackWidget(const QString& imagePath, QWidget* parent = 0);
    explicit BackWidget(const QString& imagePath, Qt::Alignment alignment, QWidget* parent = 0);

    Qt::Alignment alignment() const { return _alignment; }
    void setAlignment(Qt::Alignment alignment);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap _background;
    Qt::Alignment _alignment;
};

} // namespace Widgets
} // namespace Ori

#endif // ORIBACKWIDGET

