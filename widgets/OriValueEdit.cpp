#include "OriValueEdit.h"

#include <QDoubleValidator>
#include <QKeyEvent>

namespace Ori {
namespace Widgets {

class ValueValidator : public QDoubleValidator
{
public:
    ValueValidator(QObject* parent = 0) : QDoubleValidator(parent)
    {
        setLocale(QLocale::C);
    }

    State validate(QString& input, int& pos) const override
    {
        // use both point and comma as decimal separators
        if (input[pos-1] == ',') input[pos-1] = '.';
        return QDoubleValidator::validate(input, pos);
    }
};


ValueEdit::ValueEdit(QWidget *parent) : QLineEdit(parent)
{
    _locale = QLocale(QLocale::C);

    setAlignment(Qt::AlignRight);
    setSizePolicy(QSizePolicy::Preferred, sizePolicy().verticalPolicy());
    setValidator(new ValueValidator(this));

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(textEdited(QString)));
}

ValueEdit::ValueEdit(const double &value, QWidget *parent) : ValueEdit(parent)
{
    setValue(value);
    selectAll();
}

void ValueEdit::setValue(const double& value)
{
    setText(toString(value));
    _ok = true;
    _value = value;
    indicateValidation(true);
}

void ValueEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focused(true);
}

void ValueEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focused(false);
}

void ValueEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);
    emit keyPressed(e->key());
}

void ValueEdit::textChanged(const QString& text)
{
    processInput(text);

    if (_ok)
        emit valueChanged(_value);
}

void ValueEdit::textEdited(const QString& text)
{
    processInput(text);

    if (_ok)
        emit valueEdited(_value);
}

void ValueEdit::processInput(const QString& text)
{
    bool ok;
    double value = _locale.toDouble(text, &ok);

    if (ok != _ok)
        indicateValidation(ok);

    _ok = ok;

    if (_ok)
        _value = value;
}

QString ValueEdit::toString(const double& value) const
{
    auto s = _locale.toString(value);
    // thousand separator for C-locale is comma, need not it
    return s.replace(_locale.groupSeparator(), QString());
}

void ValueEdit::indicateValidation(bool ok)
{
    QPalette p;
    if (!ok)
        p.setColor(QPalette::Base, invalidColor(p.color(QPalette::Base)));
    setPalette(p);
}

QColor ValueEdit::invalidColor(const QColor& base)
{
    return QColor(base.red() * 0.7 + 255* 0.3,
                  base.green() * 0.7,
                  base.blue() * 0.7,
                  255);
}

} // namespace Widgets
} // namespace Ori
