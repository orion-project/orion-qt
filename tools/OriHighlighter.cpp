#include "OriHighlighter.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPlainTextEdit>
#include <QTextDocument>

namespace Ori {
namespace Highlighter {

//------------------------------------------------------------------------------
//                                    Spec
//------------------------------------------------------------------------------

QString Spec::storableString() const
{
    return rawCode().trimmed() + "\n\n---\n" + rawSample().trimmed();
}

//------------------------------------------------------------------------------
//                                 SpecLoader
//------------------------------------------------------------------------------

struct SpecLoader
{
private:
    QTextStream& stream;
    int lineNo = 0;
    QString key, val;
    QStringList code;
    QStringList sample;
    int sampleLineNo = -1;
    bool withRawData = false;
    QMap<int, QString> warnings;
    QMap<QString, int> ruleStarts;

    void warning(const QString& msg, int overrideLineNo = 0)
    {
        int reportedLineNo = (overrideLineNo > 0) ? overrideLineNo : lineNo;
        qWarning() << "Highlighter" << "| line" << reportedLineNo << "|" << msg;
        warnings[reportedLineNo] = msg;
    }

    bool readLine()
    {
        lineNo++;
        auto line = stream.readLine();

        if (sampleLineNo >= 0)
        {
            val = line;
            sampleLineNo++;
            return true;
        }

        if (line.startsWith(QStringLiteral("---")))
        {
            sampleLineNo = 0;
            return true;
        }

        code << line;

        line = line.trimmed();
        if (line.isEmpty() || line[0] == '#')
            return false;
        auto keyLen = line.indexOf(':');
        if (keyLen < 1)
        {
            warning("Key not found");
            return false;
        }

        key = line.left(keyLen).trimmed();
        val = line.mid(keyLen+1).trimmed();
        //qDebug() << "Highlighter" << source << "| line" << lineNo << "| key" << key << "| value" << val;
        return true;
    }

    void finalizeRule(Rule& rule, Spec* spec, const QRegularExpression::PatternOptions& opts)
    {
        if (!rule.terms.isEmpty())
        {
            rule.exprs.clear();
            foreach (const auto& term, rule.terms)
                rule.exprs << QRegularExpression(QString("\\b%1\\b").arg(term));
        }
        if (rule.multiline)
        {
            if (rule.exprs.isEmpty())
            {
                warning(QStringLiteral("Must be at least one \"expr\" when multiline"), ruleStarts[rule.name]);
                rule.multiline = false;
            }
            else if (rule.exprs.size() == 1)
                rule.exprs << QRegularExpression(rule.exprs.first());
            else if (rule.exprs.size() > 2)
                rule.exprs.resize(2);
        }
        for (auto& expr : rule.exprs)
            expr.setPatternOptions(opts);
        spec->rules << rule;
    }

public:
    explicit SpecLoader(QTextStream& stream, bool withRawData): stream(stream), withRawData(withRawData)
    {}

    bool loadMeta(Meta& meta, Spec* spec = nullptr)
    {
        bool suffice = false;
        while (!stream.atEnd())
        {
            if (!readLine())
                continue;
            if (sampleLineNo >= 0)
            {
                break;
            }
            else if (key == QStringLiteral("name"))
            {
                meta.name = val;
                suffice = true;
                if (spec && withRawData)
                    spec->raw[Spec::RAW_NAME_LINE] = lineNo;
            }
            else if (key == QStringLiteral("title"))
            {
                meta.title = val;
                if (spec && withRawData)
                    spec->raw[Spec::RAW_TITLE_LINE] = lineNo;
            }
            else if (key == QStringLiteral("rule"))
            {
                break;
            }
            else warning(QStringLiteral("Unknown key"));
        }
        if (!suffice)
            warning(QStringLiteral("Not all required top-level properties set, required: \"name\""), 1);
        return suffice;
    }

    SpecWarnings loadSpec(Spec* spec)
    {
        // Don't clear meta.source and meta.storage
        // loadSpec can be called on an existing spec to reload it from text editor
        // then Name and Title should be updated because they are set in highlighter code
        // while Source and Storage stay the same and will be used for saving updated spec
        spec->meta.name.clear();
        spec->meta.title.clear();
        spec->raw.clear();
        spec->rules.clear();

        if (!loadMeta(spec->meta, spec))
            return warnings;

        Rule rule;
        rule.name = val;
        QRegularExpression::PatternOptions opts;

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        auto skipEmptyParts = Qt::SkipEmptyParts;
    #else
        auto skipEmptyParts = QString::SkipEmptyParts;
    #endif

        while (!stream.atEnd())
        {
            if (!readLine())
                continue;
            if (sampleLineNo == 0)
            {
                if (!withRawData) break;
                // rules-to-sample separator, just skip it
            }
            else if (sampleLineNo > 0)
            {
                sample << val;
            }
            else if (key == QStringLiteral("rule"))
            {
                finalizeRule(rule, spec, opts);

                foreach (const auto& r, spec->rules)
                    if (r.name == val)
                    {
                        warning(QStringLiteral("Dupilicated rule name"));
                        warning(QStringLiteral("Dupilicated rule name"), ruleStarts[r.name]);
                    }
                rule = Rule();
                rule.name = val;
                ruleStarts[val] = lineNo;
                opts = QRegularExpression::PatternOptions();
            }
            else if (key == QStringLiteral("expr"))
            {
                if (rule.terms.isEmpty())
                {
                    QRegularExpression expr(val);
                    if (!expr.isValid())
                        warning(QStringLiteral("Invalid expression"));
                    else
                        rule.exprs << expr;
                }
                else warning(QStringLiteral("Can't have \"expr\" and \"terms\" in the same rule"));
            }
            else if (key == QStringLiteral("color"))
            {
                QColor c(val);
                if (!c.isValid())
                    warning(QStringLiteral("Invalid color value"));
                else
                    rule.format.setForeground(c);
            }
            else if (key == QStringLiteral("back"))
            {
                QColor c(val);
                if (!c.isValid())
                    warning(QStringLiteral("Invalid color value"));
                else
                    rule.format.setBackground(c);
            }
            else if (key == QStringLiteral("group"))
            {
                bool ok;
                int group = val.toInt(&ok);
                if (!ok)
                    warning(QStringLiteral("Invalid integer value"));
                else
                    rule.group = group;
            }
            else if (key == QStringLiteral("style"))
            {
                foreach (const auto& style, val.split(',', skipEmptyParts))
                {
                    auto s = style.trimmed();
                    if (s == QStringLiteral("bold"))
                        rule.format.setFontWeight(QFont::Bold);
                    else if (s == QStringLiteral("italic"))
                        rule.format.setFontItalic(true);
                    else if (s == QStringLiteral("underline"))
                        rule.format.setFontUnderline(true);
                    else if (s == QStringLiteral("strikeout"))
                        rule.format.setFontStrikeOut(true);
                    else if (s == QStringLiteral("hyperlink"))
                    {
                        rule.format.setAnchor(true);
                        rule.hyperlink = true;
                    }
                    else warning(QStringLiteral("Unknown style ") + s);
                }
            }
            else if (key == QStringLiteral("opts"))
            {
                foreach (const auto& style, val.split(',', skipEmptyParts))
                {
                    auto s = style.trimmed();
                    if (s == QStringLiteral("multiline"))
                        rule.multiline = true;
                    else if (s == QStringLiteral("ignore-case"))
                        opts.setFlag(QRegularExpression::CaseInsensitiveOption);
                    else warning(QStringLiteral("Unknown option ") + s);
                }
            }
            else if (key == QStringLiteral("terms"))
            {
                if (rule.exprs.isEmpty())
                {
                    foreach (const auto& term, val.split(',', skipEmptyParts))
                        rule.terms << term.trimmed();
                }
                else warning(QStringLiteral("Can't have \"expr\" and \"terms\" in the same rule"));
            }
            else warning(QStringLiteral("Unknown key"));
        }
        finalizeRule(rule, spec, opts);
        if (withRawData)
        {
            spec->raw[Spec::RAW_CODE] = code.join('\n');
            spec->raw[Spec::RAW_SAMPLE] = sample.join('\n');
        }
        return warnings;
    }
};

SpecWarnings loadSpec(QSharedPointer<Spec>& spec, QString* data, bool withRawData)
{
    QTextStream stream(data);
    SpecLoader loader(stream, withRawData);
    return loader.loadSpec(spec.get());
}

SpecResult createSpecFromFile(const QString& fileName, bool withRawData)
{
    SpecResult res;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "loadSpecFromFile" << fileName << "|" << file.errorString();
        res.error = file.errorString();
        return res;
    }
    QTextStream stream(&file);
    SpecLoader loader(stream, withRawData);
    res.spec.reset(new Spec);
    res.spec->meta.source = fileName;
    res.warns = loader.loadSpec(res.spec.get());
    return res;
}

//------------------------------------------------------------------------------
//                               FileStorage
//------------------------------------------------------------------------------

QVector<Meta> FileStorage::loadMetas() const
{
    QVector<Meta> metas;
    QDir dir(_dir);
    if (!dir.exists())
    {
        qWarning() << "Syntax highlighter directory doesn't exist" << dir.path();
        return metas;
    }
    //qDebug() << "Hightlighters::FileStorage: dir" << dir.path();
    for (auto& fileInfo : dir.entryInfoList())
    {
        if (fileInfo.fileName().endsWith(".phl"))
        {
            auto fileName = fileInfo.absoluteFilePath();
            QFile file(fileName);
            if (!file.open(QFile::ReadOnly | QFile::Text))
            {
                qWarning() << "Highlighter::FileStorage.loadMetas" << fileName << "|" << file.errorString();
                continue;
            }
            QTextStream stream(&file);
            SpecLoader loader(stream, false);
            Meta meta;
            if (loader.loadMeta(meta))
            {
                // The caller is responsible for assigning meta.storage
                // if it wants to track who loaded the specs (e.g. for saving updated spec)
                meta.source = fileName;
                metas << meta;
            }
            else
                qWarning() << "Highlighters::FileStorage: meta not loaded" << fileName;
        }
    }
    return metas;
}

QSharedPointer<Spec> FileStorage::loadSpec(const Meta &meta, bool withRawData) const
{
    QFile file(meta.source);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "Highlighter::FileStorage.loadSpec" << meta.source << "|" << file.errorString();
        return QSharedPointer<Spec>();
    }
    QTextStream stream(&file);
    QSharedPointer<Spec> spec(new Spec());
    SpecLoader loader(stream, withRawData);
    loader.loadSpec(spec.get());
    return spec;
}

QString FileStorage::saveSpec(const QSharedPointer<Spec>& spec)
{
    QFile file(spec->meta.source);
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        return QString("Failed to open highlighter file \"%1\" for writing: %2").arg(spec->meta.source, file.errorString());
    if (file.write(spec->storableString().toUtf8()) == -1)
        return QString("Failed to write highlighter file \"%1\": %2").arg(spec->meta.source, file.errorString());
    return "";
}

//------------------------------------------------------------------------------
//                              QrcStorage
//------------------------------------------------------------------------------

QVector<Meta> QrcStorage::loadMetas() const
{
    QVector<Meta> metas;
    foreach (const auto& fileName, _files)
    {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            qWarning() << "Highlighter::QrcStorage.loadMetas" << fileName << "|" << file.errorString();
            continue;
        }
        QTextStream stream(&file);
        SpecLoader loader(stream, false);
        Meta meta;
        if (loader.loadMeta(meta))
        {
            // The caller is responsible for assigning meta.storage
            // if it wants to track who loaded the specs (e.g. for saving updated spec)
            meta.source = fileName;
            metas << meta;
        }
        else
            qWarning() << "Highlighters::QrcStorage: meta not loaded" << fileName;
    }
    return metas;
}

QSharedPointer<Spec> QrcStorage::loadSpec(const Meta &meta, bool withRawData) const
{
    QFile file(meta.source);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "Highlighter::QrcStorage.loadSpec" << meta.source << "|" << file.errorString();
        return QSharedPointer<Spec>();
    }
    QTextStream stream(&file);
    QSharedPointer<Spec> spec(new Spec());
    SpecLoader loader(stream, withRawData);
    loader.loadSpec(spec.get());
    return spec;
}

//------------------------------------------------------------------------------
//                                 Highlighter
//------------------------------------------------------------------------------

Highlighter::Highlighter(QTextDocument *parent, const QSharedPointer<Spec>& spec)
    : QSyntaxHighlighter(parent), _spec(spec), _document(parent)
{
    setObjectName(spec->meta.name);
}

Highlighter::Highlighter(QPlainTextEdit *parent, const QSharedPointer<Spec>& spec) : Highlighter(parent->document(), spec)
{
}

void Highlighter::highlightBlock(const QString &text)
{
    bool hasMultilines = false;
    foreach (const auto& rule, _spec->rules)
    {
        if (rule.multiline && rule.exprs.size() >= 1)
        {
            hasMultilines = true;
            continue;
        }
        for (const auto& expr : rule.exprs)
        {
            auto m = expr.match(text);
            while (m.hasMatch())
            {
                int pos = m.capturedStart(rule.group);
                int length = m.capturedLength(rule.group);

                // Font style is applied correctly but highlighter can't make anchors and apply tooltips.
                // We do it manually overriding event handlers in MemoEditor.
                // There is the bug but seems nobody cares: https://bugreports.qt.io/browse/QTBUG-21553
                if (rule.hyperlink)
                {
                    QTextCharFormat format(rule.format);
                    format.setAnchorHref(m.captured(rule.group));
                    setFormat(pos, length, format);
                }
                else if (rule.fontSizeDelta != 0)
                {
                    QTextCharFormat format(rule.format);
                    format.setFontPointSize(_document->defaultFont().pointSize() + rule.fontSizeDelta);
                    setFormat(pos, length, format);
                }
                else
                    setFormat(pos, length, rule.format);
                m = expr.match(text, pos + length);
            }
        }
    }
    if (hasMultilines)
    {
        int offset = 0;
        setCurrentBlockState(-1);
        int size = _spec->rules.size();
        for (int i = 0; i < size; i++)
        {
            const auto& rule = _spec->rules.at(i);
            if (!rule.multiline) continue;
            offset = matchMultiline(text, rule, i, offset);
            if (offset < 0) break;
        }
    }
}

int Highlighter::matchMultiline(const QString &text, const Rule& rule, int ruleIndex, int initialOffset)
{
    const auto& exprBeg = rule.exprs[0];
    const auto& exprEnd = rule.exprs[1];
    QRegularExpressionMatch m;

    //qDebug() << rule.name << previousBlockState() << "|" << initialOffset << "|" << text;

    int start = 0;
    int offset = initialOffset;
    bool matchEnd = previousBlockState() == ruleIndex;
    while (true)
    {
        m = (matchEnd ? exprEnd : exprBeg).match(text, offset);
        if (m.hasMatch())
        {
            if (matchEnd)
            {
                setFormat(start, m.capturedEnd()-start, rule.format);
                setCurrentBlockState(-1);
                matchEnd = false;
                //qDebug() << "    has-match(end)" << start << m.capturedEnd()-start;
            }
            else
            {
                start = m.capturedStart();
                matchEnd = true;
                //qDebug() << "    has-match(beg)" << start;
            }
            offset = m.capturedEnd();
            //qDebug() << "    offset" << offset;
        }
        else
        {
            if (matchEnd)
            {
                //qDebug() << "    no-match(end)" << start << text.length()-start;
                setFormat(start, text.length()-start, rule.format);
                setCurrentBlockState(ruleIndex);
                offset = -1;
            }
            else
            {
                //qDebug() << "    no-match(beg)";
            }
            break;
        }
    }
    //qDebug() << "    return" << offset;
    return offset;
}

Highlighter *setHighlighter(QPlainTextEdit* editor, const QString& fileName)
{
    auto res = createSpecFromFile(fileName, false);
    if (!res.spec)
    {
        qWarning() << "Unable to set highighter" << fileName << res.error;
        return nullptr;
    }
    return new Highlighter(editor->document(), res.spec);
}

} // namespace Highlighter
} // namespace Ori
