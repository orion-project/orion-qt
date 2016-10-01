#ifndef ORI_TRANSLATOR_H
#define ORI_TRANSLATOR_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTranslator;
QT_END_NAMESPACE

namespace Ori {

////////////////////////////////////////////////////////////////////////////////

class TranslatorSetup
{
public:
    QString languagesDir;
    QString langFilePrefix;
    QString langFileSuffix;
    QString defaultLang;
    QString defaultLangName;
    bool applyImmediately;

    QString langFileName(const QString& lang) const;
    QString qtLangFileName(const QString& lang) const;
    bool isDefault(const QString& lang) const;
};

////////////////////////////////////////////////////////////////////////////////

class Translator : public QObject
{
    Q_OBJECT

public:
    explicit Translator(QObject *parent = 0);
    explicit Translator(TranslatorSetup setup, QObject *parent = 0);
    explicit Translator(const QString& lang, QObject *parent = 0);
    explicit Translator(const QString& lang, TranslatorSetup setup, QObject *parent = 0);

    const QString& currentLanguage() const { return _currentLang; }
    void setCurrentLanguage(const QString& lang);

    QStringList getLanguages() const;
    QString getLanguageName(const QString& lang) const;

    bool isApplyImmediately() const { return _setup.applyImmediately; }

    static TranslatorSetup defaultSetup();
    static TranslatorSetup immediateSetup();

private:
    QString _currentLang;
    TranslatorSetup _setup;
    QTranslator* _translator = nullptr;
    QTranslator* _translatorQt = nullptr;

    void applyLanguage();
    void resetTranslator();
    void setTranslator(const QString& lang);
};

////////////////////////////////////////////////////////////////////////////////

} // namespace Ori

#endif // ORI_TRANSLATOR_H
