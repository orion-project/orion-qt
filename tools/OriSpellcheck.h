#ifndef ORI_SPELLCHECK_H
#define ORI_SPELLCHECK_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QPlainTextEdit;
class QTextEdit;
QT_END_NAMESPACE

namespace Ori {

class Spellcheck : public QObject
{
    Q_OBJECT

public:
    explicit Spellcheck(QTextEdit* editor);
    explicit Spellcheck(QPlainTextEdit* editor);

    QString lang() const { return _lang; }
    void setLang(const QString &lang);
    
private:
    QWidget *_editor;
    QString _lang;
    void *_impl = nullptr;
};

class SpellcheckControl : public QObject
{
    Q_OBJECT

public:
    explicit SpellcheckControl(QObject* parent = nullptr);

    QMenu* makeMenu(QWidget* parent = nullptr);

    void showCurrentLang(const QString& lang);
    void setEnabled(bool on);

signals:
    void langSelected(const QString& lang);

private:
    QActionGroup* _actionGroup = nullptr;

    void actionGroupTriggered(QAction* action);
};

} // namespace Ori

#endif // ORI_SPELLCHECK_H
