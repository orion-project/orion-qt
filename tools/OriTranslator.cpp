#include "OriTranslator.h"

#include <QApplication>
#include <QDir>
#include <QTranslator>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    #define QStringLiteral QString::fromLatin1
#endif

namespace Ori {

////////////////////////////////////////////////////////////////////////////////
//                              TranslatorSetup
////////////////////////////////////////////////////////////////////////////////

QString TranslatorSetup::langFileName(const QString& lang) const
{
    return langFilePrefix % lang % langFileSuffix;
}

QString TranslatorSetup::qtLangFileName(const QString& lang) const
{
    return QStringLiteral("qt_") % lang % langFileSuffix;
}

bool TranslatorSetup::isDefault(const QString& lang) const
{
    return lang == defaultLang;
}

////////////////////////////////////////////////////////////////////////////////
//                              Translator
////////////////////////////////////////////////////////////////////////////////

TranslatorSetup Translator::defaultSetup()
{
    TranslatorSetup setup;
    QFileInfo appfile(qApp->applicationFilePath());
    setup.languagesDir = appfile.dir().path() % "/languages";
    setup.langFilePrefix = appfile.baseName() % "_";
    setup.langFileSuffix = QStringLiteral(".qm");
    setup.defaultLang = QStringLiteral("en");
    setup.defaultLangName = QStringLiteral("English");
    setup.applyImmediately = false;
    return setup;
}

TranslatorSetup Translator::immediateSetup()
{
    auto setup = defaultSetup();
    setup.applyImmediately = true;
    return setup;
}

Translator::Translator(QObject *parent) : Translator(defaultSetup(), parent)
{
}

Translator::Translator(TranslatorSetup setup, QObject *parent) : QObject(parent)
{
    _setup = setup;
    _currentLang = _setup.defaultLang;
}

Translator::Translator(const QString& lang, QObject *parent) : Translator(lang, defaultSetup(), parent)
{
}

Translator::Translator(const QString& lang, TranslatorSetup setup, QObject *parent) : Translator(setup, parent)
{
    _currentLang = lang.isEmpty()? setup.defaultLang: lang;

    applyLanguage();
}

void Translator::setCurrentLanguage(const QString& lang)
{
    if (_currentLang == lang) return;

    _currentLang = lang;

    if (_setup.applyImmediately)
        applyLanguage();
}

void Translator::applyLanguage()
{
    if (_setup.isDefault(_currentLang))
        resetTranslator();
    else
        setTranslator(_currentLang);
}

void Translator::resetTranslator()
{
    if (_translator)
    {
        qApp->removeTranslator(_translator);
        delete _translator;
        _translator = nullptr;
    }
    if (_translatorQt)
    {
        qApp->removeTranslator(_translatorQt);
        delete _translatorQt;
        _translatorQt = nullptr;
    }
}

void Translator::setTranslator(const QString& lang)
{
    if (!_translator)
        qApp->installTranslator(_translator = new QTranslator(qApp));

    if (!_translatorQt)
        qApp->installTranslator(_translatorQt = new QTranslator(qApp));

    _translator->load(_setup.langFileName(lang), _setup.languagesDir);
    _translatorQt->load(_setup.qtLangFileName(lang), _setup.languagesDir);
}

QStringList Translator::getLanguages() const
{
    QDir dir(_setup.languagesDir);
    QStringList files = dir.entryList(QStringList(_setup.langFileName("*")));
    QStringList langs;
    for (const QString& file: files)
    {
        int prefixLen = _setup.langFilePrefix.length();
        int suffixLen = _setup.langFileSuffix.length();
        langs << file.mid(prefixLen, file.length() - prefixLen - suffixLen);
    }
    langs << _setup.defaultLang;
    langs.sort();
    return langs;
}

QString Translator::getLanguageName(const QString& lang) const
{
    if (_setup.isDefault(lang))
        return _setup.defaultLangName;

    QTranslator tr; tr.load(_setup.langFileName(lang), _setup.languagesDir);

    QString langName = tr.translate("Language", "English", "Translate this to the "
        "language of the translation file, NOT to the meaning of 'English'!");

    return langName.isEmpty()? lang: langName;
}

} // namespace Ori
