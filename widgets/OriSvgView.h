#ifndef ORI_SVG_VIEW_H
#define ORI_SVG_VIEW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSvgRenderer;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {

/// Same as QSvgWidget, but SvgView paints image in proportional mode at widget center,
/// whereas QSvgWidget paints image stretched to whole widget area.
class SvgView : public QWidget
{
  Q_OBJECT

public:
    explicit SvgView(QWidget *parent = nullptr);
    explicit SvgView(const QString &prefix, QWidget *parent = nullptr);

    QSize sizeHint() const override;

    static SvgView* makeStatic(const QString& path, QWidget *parent = nullptr);

public slots:
    void clear();
    void load(const QString& resource);

protected:
  virtual void paintEvent(QPaintEvent *event) override;

private:
    QSvgRenderer *renderer;
    float aspect; // sets on load
    QString prefix;

    void updateAspect();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_SVG_VIEW_H
