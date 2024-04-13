#ifndef ORI_LABELS_H
#define ORI_LABELS_H

#include <QLabel>

namespace Ori {
namespace Widgets {

//------------------------------------------------------------------------------
//                                  Label
//------------------------------------------------------------------------------

class Label : public QLabel
{
    Q_OBJECT

public:
    explicit Label(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit Label(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void setSizeHint(QSize size);
    void setSizeHint(int w, int h);
    void setHorizontalSizeHint(int w);
    void setVerticalSizeHint(int h);
    void setHorizontalSizePolicy(QSizePolicy::Policy policy);
    void setVerticalSizePolicy(QSizePolicy::Policy policy);
    QSize sizeHint() const override { return _sizeHint; }

signals:
    void clicked();
    void doubleClicked();

protected:
    void mouseReleaseEvent(class QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    QSize _sizeHint;
};

//------------------------------------------------------------------------------
//                             LabelSeparator
//------------------------------------------------------------------------------

class LabelSeparator : public QLabel
{
    Q_OBJECT

public:
    explicit LabelSeparator(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit LabelSeparator(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit LabelSeparator(const QString &text, bool bold);

    bool flat = false;
    int textIndent = 10;

protected:
    void paintEvent(QPaintEvent*);
};

//------------------------------------------------------------------------------
//                               ImagedLabel
//------------------------------------------------------------------------------

class ImagedLabel : public QWidget
{
    Q_OBJECT

public:
    explicit ImagedLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit ImagedLabel(const QString &text, const QString &imagePath);

    void setContent(const QString& text, const QString& imagePath = QString());

    QLabel *iconLabel() const { return _icon; }
    QLabel *textLabel() const { return _text; }

    void clear();

private:
    QLabel *_icon, *_text;
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_LABELS_H
