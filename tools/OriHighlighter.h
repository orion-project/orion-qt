#ifndef ORI_HIGHLIGHTER_H
#define ORI_HIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Ori {
namespace Highlighter {


class SpecStorage;


struct Meta
{
    QString name;
    QString title;
    QString source;
    QSharedPointer<SpecStorage> storage;

    QString displayTitle() const { return title.isEmpty() ? name : title; }
};


struct Rule
{
    QString name;
    QVector<QRegularExpression> exprs;
    QTextCharFormat format;
    QStringList terms;
    int group = 0;
    bool hyperlink = false;
    bool multiline = false;
    int fontSizeDelta = 0;
};


struct Spec
{
    Meta meta;
    QVector<Rule> rules;

    // not empty only when spec is loaded withRawData
    // this stuff is required for highlighter editor
    QMap<int, QVariant> raw;
    enum RawData {RAW_CODE, RAW_SAMPLE, RAW_NAME_LINE, RAW_TITLE_LINE};
    QString rawCode() const { return raw[RAW_CODE].toString(); }
    QString rawSample() const { return raw[RAW_SAMPLE].toString(); }
    void setRawSample(const QString& s) { raw[RAW_SAMPLE] = s; }
    int rawNameLineNo() const { return raw[RAW_NAME_LINE].toInt(); }
    int rawTitleLineNo() const { return raw[RAW_TITLE_LINE].toInt(); }
    QString storableString() const;
};


class SpecStorage
{
public:
    virtual ~SpecStorage() {}
    virtual QString name() const = 0;
    virtual bool readOnly() const = 0;
    virtual QVector<Meta> loadMetas() const = 0;
    virtual QSharedPointer<Spec> loadSpec(const Meta& meta, bool withRawData = false) const = 0;
    virtual QString saveSpec(const QSharedPointer<Spec>& spec) = 0;
    virtual QString deleteSpec(const Meta& meta) = 0;
};


class FileStorage : public SpecStorage
{
public:
    FileStorage(const QString& dir): _dir(dir) {}
    QString name() const override { return QStringLiteral("default-storage"); }
    bool readOnly() const override { return false; }
    QVector<Meta> loadMetas() const override;
    QSharedPointer<Spec> loadSpec(const Meta &meta, bool withRawData = false) const override;
    QString saveSpec(const QSharedPointer<Spec>& spec) override;
    QString deleteSpec(const Meta&) override { return QString(); }
private:
    QString _dir;
};


class QrcStorage : public SpecStorage
{
public:
    QrcStorage(const QVector<QString>& files): _files(files) {}
    QString name() const override { return QStringLiteral("qrc-storage"); }
    bool readOnly() const override { return true; }
    QVector<Meta> loadMetas() const override;
    QSharedPointer<Spec> loadSpec(const Meta &meta, bool withRawData = false) const override;
    QString saveSpec(const QSharedPointer<Spec>&) override { return QString(); }
    QString deleteSpec(const Meta&) override { return QString(); }
private:
    QVector<QString> _files;
};

using SpecLineNo = int;
using SpecWarnings = QMap<SpecLineNo, QString>;

struct SpecResult
{
    QSharedPointer<Spec> spec;
    QString error;
    SpecWarnings warns;
};

SpecWarnings loadSpec(QSharedPointer<Spec>& spec, QString* data, bool withRawData);
SpecResult createSpecFromFile(const QString& fileName, bool withRawData);

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit Highlighter(QTextDocument *parent, const QSharedPointer<Spec>& spec);
    explicit Highlighter(QPlainTextEdit *parent, const QSharedPointer<Spec>& spec);

protected:
    void highlightBlock(const QString &text);

private:
    QSharedPointer<Spec> _spec;
    QTextDocument* _document;

    int matchMultiline(const QString &text, const Rule& rule, int ruleIndex, int initialOffset);
};

Highlighter* setHighlighter(QPlainTextEdit* editor, const QString& fileName);

} // namespace Highlighter
} // namespace Ori

#endif // ORI_HIGHLIGHTER_H
