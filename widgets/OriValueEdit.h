#ifndef ORI_VALUE_EDIT_H
#define ORI_VALUE_EDIT_H

#include <QLineEdit>
#include <QLocale>

namespace Ori {
namespace Widgets {

class ValueEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit ValueEdit(QWidget *parent = 0);
    explicit ValueEdit(const double& value, QWidget *parent = 0);

    double value() const { return _value; }
    void setValue(const double &value);
    bool ok() const { return _ok; }

    QSize sizeHint() const { return QSize(128, 21); }

signals:
    void focused(bool focus);
    void keyPressed(int key);
    void valueEdited(double value);
    void valueChanged(double value);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);

private:
    QLocale _locale;
    double _value = 0;
    bool _ok;

    QString toString(const double& value) const;

    void processInput(const QString& text);
    void indicateValidation(bool ok);
    QColor invalidColor(const QColor& base);

private slots:
    void textChanged(const QString& text);
    void textEdited(const QString& text);
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_VALUE_EDIT_H
