#include "OriSpellcheckEngine.h"

#include "tools/OriSettings.h"

#include "hunspell/hunspell.hxx"

#include <QApplication>
#include <QDir>
#include <QRegularExpression>
#include <QTextCodec>

namespace Ori {

namespace  {
static const QString dictFileExt(".dic");
static const QString affixFileExt(".aff");
}

static QDir dictionaryDir()
{
    QDir dir(qApp->applicationDirPath() + "/dicts");

#ifdef Q_OS_MAC
    if (!dir.exists())
    {
        // Look near the application bundle, it is for development mode
        return QDir(qApp->applicationDirPath() % "/../../../dicts");
    }
#endif

    return dir;
}

static QString userDictionaryPath(const QString& lang)
{
    QSharedPointer<QSettings> s(Ori::Settings::open());
    QDir dictDir = QFileInfo(s->fileName()).absoluteDir();
    QFileInfo dictFile(dictDir, "userdict-" + lang + dictFileExt);
    return dictFile.absoluteFilePath();
}

// detect encoding analyzing the SET option in the affix file
static QString dictionaryEncoding(const QString& affixFilePath)
{
    QFile file(affixFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open affix file" << affixFilePath << file.errorString();
        return QString();
    }
    QString encoding;
    QTextStream stream(&file);
    QRegularExpression enc_detector("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*", QRegularExpression::CaseInsensitiveOption);
    for (QString line = stream.readLine(); !line.isEmpty(); line = stream.readLine())
    {
        auto m = enc_detector.match(line);
        if (m.hasMatch())
        {
            encoding = m.captured(1);
            break;
        }
    }
    file.close();
    return encoding;
}

QStringList SpellcheckEngine::dictionaries()
{
    QStringList dicts;

    for (auto& fileInfo : dictionaryDir().entryInfoList())
        if (fileInfo.fileName().endsWith(dictFileExt))
            dicts << fileInfo.baseName();

    return dicts;
}

SpellcheckEngine* SpellcheckEngine::get(const QString& lang)
{
    if (lang.isEmpty())
    {
        qWarning() << "Language code is empty";
        return nullptr;
    }

    static QMap<QString, SpellcheckEngine*> checkers;

    if (!checkers.contains(lang))
    {
        QDir dictDir = dictionaryDir();

        QFileInfo dictFile(dictDir, lang + dictFileExt);
        if (!dictFile.exists())
        {
            qWarning() << "Dictionary file does not exist" << dictFile.filePath();
            return nullptr;
        }

        QFileInfo affixFile(dictDir, lang + affixFileExt);
        if (!affixFile.exists())
        {
            qWarning() << "Affix file does not exist" << affixFile.filePath();
            return nullptr;
        }

        auto checker = new SpellcheckEngine(dictFile.absoluteFilePath(),
                                            affixFile.absoluteFilePath(),
                                            userDictionaryPath(lang));
        if (!checker->_hunspell)
        {
            delete checker;
            return nullptr;
        }

        checker->_lang = lang;
        checkers.insert(lang, checker);
    }

    return checkers[lang];
}


SpellcheckEngine::SpellcheckEngine(const QString &dictFilePath, const QString& affixFilePath, const QString &userDictionaryPath)
{
    _userDictionaryPath = userDictionaryPath;

    QString encoding = dictionaryEncoding(affixFilePath);
    if (encoding.isEmpty())
    {
        qWarning() << "Unable to detect dictionary encoding in affix file"
                   << affixFilePath << "Spellcheck is unavailable";
        return;
    }

    _codec = QTextCodec::codecForName(encoding.toLatin1().constData());
    if (!_codec)
    {
        qWarning() << "Codec not found for encoding" << encoding
                   << "detected in dictionary" << affixFilePath
                   << "Spellcheck is unavailable";
        return;
    }

    _hunspell = new Hunspell(affixFilePath.toLocal8Bit().constData(),
                             dictFilePath.toLocal8Bit().constData());

    loadUserDictionary();
}

SpellcheckEngine::~SpellcheckEngine()
{
    if (_hunspell) delete _hunspell;
}

bool SpellcheckEngine::check(const QString &word) const
{
    return _hunspell->spell(_codec->fromUnicode(word).toStdString());
}

void SpellcheckEngine::ignore(const QString &word)
{
    _hunspell->add(_codec->fromUnicode(word).toStdString());
    emit wordIgnored(word);
}

void SpellcheckEngine::save(const QString &word)
{
    if (_userDictionaryPath.isEmpty()) return;

    QFile file(_userDictionaryPath);
    if (!file.open(QIODevice::Append))
    {
        qWarning() << "Unable to open user dictionary file for writing"
                   << _userDictionaryPath << file.errorString();
        return;
    }

    QTextStream stream(&file);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
    stream << word << "\n";
    file.close();
}

QStringList SpellcheckEngine::suggest(const QString &word) const
{
    QStringList variants;
    for (auto& variant : _hunspell->suggest(_codec->fromUnicode(word).toStdString()))
        variants << _codec->toUnicode(QByteArray::fromStdString(variant));
    return variants;
}

void SpellcheckEngine::loadUserDictionary()
{
    if (_userDictionaryPath.isEmpty()) return;

    QFile file(_userDictionaryPath);
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open user dictionary file for reading"
                   << _userDictionaryPath << file.errorString();
        return;
    }

    QTextStream stream(&file);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif
    for (QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine())
        _hunspell->add(_codec->fromUnicode(word).toStdString());
    file.close();
}

// http://www.lingoes.net/en/translator/langcode.htm
QHash<QString, QString> SpellcheckEngine::langNamesMap()
{
    return
    {
        {"af", "Afrikaans"},
        {"af_ZA", "Afrikaans (South Africa)"},
        {"ar", "Arabic"},
        {"ar_AE", "Arabic (U.A.E.)"},
        {"ar_BH", "Arabic (Bahrain)"},
        {"ar_DZ", "Arabic (Algeria)"},
        {"ar_EG", "Arabic (Egypt)"},
        {"ar_IQ", "Arabic (Iraq)"},
        {"ar_JO", "Arabic (Jordan)"},
        {"ar_KW", "Arabic (Kuwait)"},
        {"ar_LB", "Arabic (Lebanon)"},
        {"ar_LY", "Arabic (Libya)"},
        {"ar_MA", "Arabic (Morocco)"},
        {"ar_OM", "Arabic (Oman)"},
        {"ar_QA", "Arabic (Qatar)"},
        {"ar_SA", "Arabic (Saudi Arabia)"},
        {"ar_SY", "Arabic (Syria)"},
        {"ar_TN", "Arabic (Tunisia)"},
        {"ar_YE", "Arabic (Yemen)"},
        {"az", "Azeri (Latin)"},
        {"az_AZ", "Azeri (Latin) (Azerbaijan)"},
        {"az_AZ", "Azeri (Cyrillic) (Azerbaijan)"},
        {"be", "Belarusian"},
        {"be_BY", "Belarusian (Belarus)"},
        {"bg", "Bulgarian"},
        {"bg_BG", "Bulgarian (Bulgaria)"},
        {"bs_BA", "Bosnian (Bosnia and Herzegovina)"},
        {"ca", "Catalan"},
        {"ca_ES", "Catalan (Spain)"},
        {"cs", "Czech"},
        {"cs_CZ", "Czech (Czech Republic)"},
        {"cy", "Welsh"},
        {"cy_GB", "Welsh (United Kingdom)"},
        {"da", "Danish"},
        {"da_DK", "Danish (Denmark)"},
        {"de", "German"},
        {"de_AT", "German (Austria)"},
        {"de_CH", "German (Switzerland)"},
        {"de_DE", "German (Germany)"},
        {"de_LI", "German (Liechtenstein)"},
        {"de_LU", "German (Luxembourg)"},
        {"dv", "Divehi"},
        {"dv_MV", "Divehi (Maldives)"},
        {"el", "Greek"},
        {"el_GR", "Greek (Greece)"},
        {"en", "English"},
        {"en_AU", "English (Australia)"},
        {"en_BZ", "English (Belize)"},
        {"en_CA", "English (Canada)"},
        {"en_CB", "English (Caribbean)"},
        {"en_GB", "English (United Kingdom)"},
        {"en_IE", "English (Ireland)"},
        {"en_JM", "English (Jamaica)"},
        {"en_NZ", "English (New Zealand)"},
        {"en_PH", "English (Republic of the Philippines)"},
        {"en_TT", "English (Trinidad and Tobago)"},
        {"en_US", "English (United States)"},
        {"en_ZA", "English (South Africa)"},
        {"en_ZW", "English (Zimbabwe)"},
        {"eo", "Esperanto"},
        {"es", "Spanish"},
        {"es_AR", "Spanish (Argentina)"},
        {"es_BO", "Spanish (Bolivia)"},
        {"es_CL", "Spanish (Chile)"},
        {"es_CO", "Spanish (Colombia)"},
        {"es_CR", "Spanish (Costa Rica)"},
        {"es_DO", "Spanish (Dominican Republic)"},
        {"es_EC", "Spanish (Ecuador)"},
        {"es_ES", "Spanish (Castilian)"},
        {"es_ES", "Spanish (Spain)"},
        {"es_GT", "Spanish (Guatemala)"},
        {"es_HN", "Spanish (Honduras)"},
        {"es_MX", "Spanish (Mexico)"},
        {"es_NI", "Spanish (Nicaragua)"},
        {"es_PA", "Spanish (Panama)"},
        {"es_PE", "Spanish (Peru)"},
        {"es_PR", "Spanish (Puerto Rico)"},
        {"es_PY", "Spanish (Paraguay)"},
        {"es_SV", "Spanish (El Salvador)"},
        {"es_UY", "Spanish (Uruguay)"},
        {"es_VE", "Spanish (Venezuela)"},
        {"et", "Estonian"},
        {"et_EE", "Estonian (Estonia)"},
        {"eu", "Basque"},
        {"eu_ES", "Basque (Spain)"},
        {"fa", "Farsi"},
        {"fa_IR", "Farsi (Iran)"},
        {"fi", "Finnish"},
        {"fi_FI", "Finnish (Finland)"},
        {"fo", "Faroese"},
        {"fo_FO", "Faroese (Faroe Islands)"},
        {"fr", "French"},
        {"fr_BE", "French (Belgium)"},
        {"fr_CA", "French (Canada)"},
        {"fr_CH", "French (Switzerland)"},
        {"fr_FR", "French (France)"},
        {"fr_LU", "French (Luxembourg)"},
        {"fr_MC", "French (Principality of Monaco)"},
        {"gl", "Galician"},
        {"gl_ES", "Galician (Spain)"},
        {"gu", "Gujarati"},
        {"gu_IN", "Gujarati (India)"},
        {"he", "Hebrew"},
        {"he_IL", "Hebrew (Israel)"},
        {"hi", "Hindi"},
        {"hi_IN", "Hindi (India)"},
        {"hr", "Croatian"},
        {"hr_BA", "Croatian (Bosnia and Herzegovina)"},
        {"hr_HR", "Croatian (Croatia)"},
        {"hu", "Hungarian"},
        {"hu_HU", "Hungarian (Hungary)"},
        {"hy", "Armenian"},
        {"hy_AM", "Armenian (Armenia)"},
        {"id", "Indonesian"},
        {"id_ID", "Indonesian (Indonesia)"},
        {"is", "Icelandic"},
        {"is_IS", "Icelandic (Iceland)"},
        {"it", "Italian"},
        {"it_CH", "Italian (Switzerland)"},
        {"it_IT", "Italian (Italy)"},
        {"ja", "Japanese"},
        {"ja_JP", "Japanese (Japan)"},
        {"ka", "Georgian"},
        {"ka_GE", "Georgian (Georgia)"},
        {"kk", "Kazakh"},
        {"kk_KZ", "Kazakh (Kazakhstan)"},
        {"kn", "Kannada"},
        {"kn_IN", "Kannada (India)"},
        {"ko", "Korean"},
        {"ko_KR", "Korean (Korea)"},
        {"kok", "Konkani"},
        {"kok_IN", "Konkani (India)"},
        {"ky", "Kyrgyz"},
        {"ky_KG", "Kyrgyz (Kyrgyzstan)"},
        {"lt", "Lithuanian"},
        {"lt_LT", "Lithuanian (Lithuania)"},
        {"lv", "Latvian"},
        {"lv_LV", "Latvian (Latvia)"},
        {"mi", "Maori"},
        {"mi_NZ", "Maori (New Zealand)"},
        {"mk", "FYRO Macedonian"},
        {"mk_MK", "FYRO Macedonian (Former Yugoslav Republic of Macedonia)"},
        {"mn", "Mongolian"},
        {"mn_MN", "Mongolian (Mongolia)"},
        {"mr", "Marathi"},
        {"mr_IN", "Marathi (India)"},
        {"ms", "Malay"},
        {"ms_BN", "Malay (Brunei Darussalam)"},
        {"ms_MY", "Malay (Malaysia)"},
        {"mt", "Maltese"},
        {"mt_MT", "Maltese (Malta)"},
        {"nb", "Norwegian (Bokm?l)"},
        {"nb_NO", "Norwegian (Bokm?l) (Norway)"},
        {"nl", "Dutch"},
        {"nl_BE", "Dutch (Belgium)"},
        {"nl_NL", "Dutch (Netherlands)"},
        {"nn_NO", "Norwegian (Nynorsk) (Norway)"},
        {"ns", "Northern Sotho"},
        {"ns_ZA", "Northern Sotho (South Africa)"},
        {"pa", "Punjabi"},
        {"pa_IN", "Punjabi (India)"},
        {"pl", "Polish"},
        {"pl_PL", "Polish (Poland)"},
        {"ps", "Pashto"},
        {"ps_AR", "Pashto (Afghanistan)"},
        {"pt", "Portuguese"},
        {"pt_BR", "Portuguese (Brazil)"},
        {"pt_PT", "Portuguese (Portugal)"},
        {"qu", "Quechua"},
        {"qu_BO", "Quechua (Bolivia)"},
        {"qu_EC", "Quechua (Ecuador)"},
        {"qu_PE", "Quechua (Peru)"},
        {"ro", "Romanian"},
        {"ro_RO", "Romanian (Romania)"},
        {"ru", "Russian"},
        {"ru_RU", "Russian (Russia)"},
        {"sa", "Sanskrit"},
        {"sa_IN", "Sanskrit (India)"},
        {"se", "Sami (Northern)"},
        {"se_FI", "Sami (Northern) (Finland)"},
        {"se_FI", "Sami (Skolt) (Finland)"},
        {"se_FI", "Sami (Inari) (Finland)"},
        {"se_NO", "Sami (Northern) (Norway)"},
        {"se_NO", "Sami (Lule) (Norway)"},
        {"se_NO", "Sami (Southern) (Norway)"},
        {"se_SE", "Sami (Northern) (Sweden)"},
        {"se_SE", "Sami (Lule) (Sweden)"},
        {"se_SE", "Sami (Southern) (Sweden)"},
        {"sk", "Slovak"},
        {"sk_SK", "Slovak (Slovakia)"},
        {"sl", "Slovenian"},
        {"sl_SI", "Slovenian (Slovenia)"},
        {"sq", "Albanian"},
        {"sq_AL", "Albanian (Albania)"},
        {"sr_BA", "Serbian (Latin) (Bosnia and Herzegovina)"},
        {"sr_BA", "Serbian (Cyrillic) (Bosnia and Herzegovina)"},
        {"sr_SP", "Serbian (Latin) (Serbia and Montenegro)"},
        {"sr_SP", "Serbian (Cyrillic) (Serbia and Montenegro)"},
        {"sv", "Swedish"},
        {"sv_FI", "Swedish (Finland)"},
        {"sv_SE", "Swedish (Sweden)"},
        {"sw", "Swahili"},
        {"sw_KE", "Swahili (Kenya)"},
        {"syr", "Syriac"},
        {"syr_SY", "Syriac (Syria)"},
        {"ta", "Tamil"},
        {"ta_IN", "Tamil (India)"},
        {"te", "Telugu"},
        {"te_IN", "Telugu (India)"},
        {"th", "Thai"},
        {"th_TH", "Thai (Thailand)"},
        {"tl", "Tagalog"},
        {"tl_PH", "Tagalog (Philippines)"},
        {"tn", "Tswana"},
        {"tn_ZA", "Tswana (South Africa)"},
        {"tr", "Turkish"},
        {"tr_TR", "Turkish (Turkey)"},
        {"tt", "Tatar"},
        {"tt_RU", "Tatar (Russia)"},
        {"ts", "Tsonga"},
        {"uk", "Ukrainian"},
        {"uk_UA", "Ukrainian (Ukraine)"},
        {"ur", "Urdu"},
        {"ur_PK", "Urdu (Islamic Republic of Pakistan)"},
        {"uz", "Uzbek (Latin)"},
        {"uz_UZ", "Uzbek (Latin) (Uzbekistan)"},
        {"uz_UZ", "Uzbek (Cyrillic) (Uzbekistan)"},
        {"vi", "Vietnamese"},
        {"vi_VN", "Vietnamese (Viet Nam)"},
        {"xh", "Xhosa"},
        {"xh_ZA", "Xhosa (South Africa)"},
        {"zh", "Chinese"},
        {"zh_CN", "Chinese (S)"},
        {"zh_HK", "Chinese (Hong Kong)"},
        {"zh_MO", "Chinese (Macau)"},
        {"zh_SG", "Chinese (Singapore)"},
        {"zh_TW", "Chinese (T)"},
        {"zu", "Zulu"},
        {"zu_ZA", "Zulu (South Africa)"}
    };
}

} // namespace Ori