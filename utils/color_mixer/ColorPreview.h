#ifndef COLORPREVIEW_H
#define COLORPREVIEW_H

#include <QFrame>
#include <QLineEdit>
#include <QWidget>

class ColorPreview : public QWidget
{
    Q_OBJECT

public:
    explicit ColorPreview(QWidget *parent = 0);

    const QColor& color() const { return _color; }

    QSize sizeHint() const override { return QSize(64, 96); }

public slots:
    void setColor(const QColor& color);

private:
    QColor _color;
    QLineEdit *_textHtml, *_textRgb;
    QFrame* _preview;

    QLineEdit* makeText();
};

#endif // COLORPREVIEW_H
