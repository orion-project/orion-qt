#ifndef ORI_SPELLCHECK_ENGINE_H
#define ORI_SPELLCHECK_ENGINE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTextCodec;
QT_END_NAMESPACE

class Hunspell;

namespace Ori {

class SpellcheckEngine: public QObject
{
    Q_OBJECT

public:
    static SpellcheckEngine* get(const QString& lang);

    ~SpellcheckEngine();

    const QString& lang() const { return _lang; }
    bool check(const QString &word) const;
    void ignore(const QString &word);
    void save(const QString &word);
    QStringList suggest(const QString &word) const;
    
    static QStringList dictionaries();
    static QHash<QString, QString> langNamesMap();

signals:
    void wordIgnored(const QString& word);

private:
    SpellcheckEngine(const QString &dictFilePath, const QString &affixFilePath, const QString &userDictionaryPath);

    QString _lang;
    QString _userDictionaryPath;
    Hunspell* _hunspell = nullptr;
    QTextCodec *_codec;

    void loadUserDictionary();
};

} // namespace Ori

#endif // ORI_SPELLCHECK_ENGINE_H