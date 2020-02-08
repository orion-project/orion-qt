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
    explicit ValueEdit(QWidget *parent = nullptr);
    explicit ValueEdit(const double& value, QWidget *parent = nullptr);

    double value() const { return _value; }
    void setValue(const double &value);
    bool ok() const { return _ok; }

    int numberPrecision() const { return _numberPrecision; }
    void setNumberPrecision(int value) { _numberPrecision = value; }

    QSize sizeHint() const override { return QSize(_preferredWidth, 21); }
    void setPreferredWidth(int w) { _preferredWidth = w; }

signals:
    void focused(bool focus);
    void keyPressed(int key);
    void valueEdited(double value);
    void valueChanged(double value);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    QLocale _locale;
    double _value = 0;
    bool _ok;
    int _numberPrecision = 6;
    int _preferredWidth = 128;

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
