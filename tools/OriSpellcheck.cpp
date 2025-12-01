#include "OriSpellcheck.h"

#include "tools/OriSpellcheckEngine.h"

#include <QActionGroup>
#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTextCodec>
#include <QTextEdit>
#include <QTimer>

namespace Ori {

// Hyperlink made via syntax highlighter doesn't create some 'top level' anchor,
// so `anchorAt` returns nothing, we have to enumerate styles to find out a href.
QString hyperlinkAt(const QTextCursor& cursor)
{
    int cursorPos = cursor.positionInBlock() - (cursor.position() - cursor.anchor());
    for (auto& format : cursor.block().layout()->formats())
        if (format.format.isAnchor() &&
            cursorPos >= format.start &&
            cursorPos < format.start + format.length)
        {
            auto href = format.format.anchorHref();
            if (not href.isEmpty()) return href;
        }
    return QString();
}

int selectWordAt(QTextCursor& cursor)
{
    // When move cursor to EndOfWord, it stops before bracket, qoute, or punctuation.
    // But quotes like &raquo; or &rdquo; become a part of the world for some reason.
    // Remove such punctuation at word boundaries.

    QString word = cursor.selectedText();
    
    int length = word.length();
    int start = 0;
    int stop = length - 1;

    while (start < length)
    {
        auto ch = word.at(start);
        if (ch.isLetter() || ch.isDigit()) break;
        start++;
    }

    while (stop > start)
    {
        auto ch = word.at(stop);
        if (ch.isLetter() || ch.isDigit()) break;
        stop--;
    }

    length = stop - start + 1;
    int anchor = cursor.anchor();
    cursor.setPosition(anchor + start, QTextCursor::MoveAnchor);
    cursor.setPosition(anchor + start + length, QTextCursor::KeepAnchor);
    return length;
}

//------------------------------------------------------------------------------
//                               SpellcheckImpl
//------------------------------------------------------------------------------

template <class TEditor>
class SpellcheckImpl : public QObject
{
public:
    explicit SpellcheckImpl(TEditor* editor, SpellcheckEngine* spellchecker) :
        QObject(editor), _editor(editor), _spellchecker(spellchecker)
    {
        _spellErrorFormat.setUnderlineColor(Qt::red);
        _spellErrorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    
        connect(_spellchecker, &SpellcheckEngine::wordIgnored, this, &SpellcheckImpl::wordIgnored);

        _editor->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(_editor, &TEditor::customContextMenuRequested, this, &SpellcheckImpl::contextMenuRequested);
        connect(_editor, &TEditor::cursorPositionChanged, this, &SpellcheckImpl::cursorMoved);
        connect(_editor->document(), QOverload<int, int, int>::of(&QTextDocument::contentsChange), this, &SpellcheckImpl::documentChanged);
        
        _timer = new QTimer(this);
        _timer->setInterval(500);
        connect(_timer, &QTimer::timeout, this, &SpellcheckImpl::doSpellcheck);
    }
    
    ~SpellcheckImpl()
    {
        _editor->setContextMenuPolicy(Qt::DefaultContextMenu);
    }
    
    void spellcheckAll()
    {
        _spellcheckStart = -1;
        _spellcheckStop = -1;
        spellcheck();
        _editor->setExtraSelections(_errorMarks);
    }
    
    void clearErrorMarks()
    {
        _editor->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }

private:
    TEditor* _editor;
    SpellcheckEngine* _spellchecker = nullptr;
    QTimer* _timer;
    bool _changesLocked = false;
    int _changesStart = -1;
    int _changesStop = -1;
    int _spellcheckStart = -1;
    int _spellcheckStop = -1;
    bool _isHrefChanged = false;
    QString _hrefAtCursor;
    QList<QTextEdit::ExtraSelection> _errorMarks;
    QTextCharFormat _spellErrorFormat;

    void documentChanged(int position, int charsRemoved, int charsAdded)
    {
        Q_UNUSED(charsRemoved)

        if (_changesLocked) return;
    
        if (_changesStart < 0 || position < _changesStart) _changesStart = position;
    
        int stopPos = position + charsAdded;
        if (stopPos > _changesStop) _changesStop = stopPos;
    
        _isHrefChanged = _isHrefChanged || !_hrefAtCursor.isEmpty();
    
        _timer->start();
    }
    
    void doSpellcheck()
    {
        _timer->stop();
    
        QTextCursor cursor(_editor->document());
    
        // We could insert spaces and split a word in two.
        // Then we have to check not only the current word but also the previous one.
        // That's the reason for shifting to -1 from _changesStart position.
        // But if there is/was a hyperlink in place of changes,
        // it can contain arbitrary number of words, then it better to check all the block.
        cursor.setPosition(_changesStart > 0 ? _changesStart - 1 : _changesStart);
        cursor.movePosition(_isHrefChanged ? QTextCursor::StartOfBlock : QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
        _spellcheckStart = cursor.position();
    
        cursor.setPosition(_changesStop);
        cursor.movePosition(_isHrefChanged ? QTextCursor::EndOfBlock : QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        _spellcheckStop = cursor.position();
    
        // Remove marks which are in checking range, they will be recreated on spellcheck
        QList<QTextEdit::ExtraSelection> errorMarks;
        for (auto &es : _editor->extraSelections())
            if (es.cursor.position() < _spellcheckStart ||
                es.cursor.anchor() >= _spellcheckStop)
                errorMarks << es;
    
        spellcheck();
    
        errorMarks.append(_errorMarks);
        _editor->setExtraSelections(errorMarks);
    
        _spellcheckStart = -1;
        _spellcheckStop = -1;
        _changesStart = -1;
        _changesStop = -1;
    }

    void spellcheck()
    {
        _errorMarks.clear();
    
        QTextCursor cursor(_editor->document());
    
        if (_spellcheckStart > -1) cursor.setPosition(_spellcheckStart);
    
        while ((_spellcheckStop < 0 || cursor.position() < _spellcheckStop) && !cursor.atEnd())
        {
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            int length = selectWordAt(cursor);
    
            // When we've skipped punctuation and there is no word,
            // the cursor may be at the beginning of the next word already.
            // For example, have text "(word1) word2",
            // the bracket "(" is treated by QTextEdit as a separate word.
            // After skipping it, we become at the "w" of "word1" - at the next word after the bracket!
            // Then moving to `NextWord` shifts the cursor to "word2", and "word1" gets missed.
            // To avoid, try to find the end of a (possible current) word after skipping punctuation.
            if (length == 0)
            {
                cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
                length = selectWordAt(cursor);
            }
    
            // Skip one-letter words
            //
            // TODO: currently, abbreviations such as "e.i.", "e.g.", "т.д.", "т.п."
            // are splitted to series of one-letter words and therefore skipped.
            // It allows mixing of such words in different languages,
            // e.g. one can use "т.д." in a text in English, it's not ok.
            //
            // Similar issue with words like "doesn't" - it splitted into "doesn" and "t",
            // "t" is skipped and "doesn" gives spelling error. Checking is ok when
            // true apostrophe character is used (’ = U+2019). But it's not the case
            // when one type memos via keyboard - single quote is generally used as apostrophe.
            //
            // It'd be better to check such words as a whole.
            //
            if (length > 1 && hyperlinkAt(cursor).isEmpty())
            {
                QString word = cursor.selectedText();
    
                if (!_spellchecker->check(word))
                    _errorMarks << QTextEdit::ExtraSelection {cursor, _spellErrorFormat};
            }
    
            cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor);
        }
    }
    
    void cursorMoved()
    {
        _hrefAtCursor = hyperlinkAt(_editor->textCursor());
    }

    void wordIgnored(const QString& word)
    {
        QList<QTextEdit::ExtraSelection> errorMarks;
        for (auto &es : _editor->extraSelections())
            if (es.cursor.selectedText() != word)
                errorMarks << es;
        _editor->setExtraSelections(errorMarks);
    }
    
    void contextMenuRequested(const QPoint &pos)
    {
        auto menu = _editor->createStandardContextMenu(pos);
    
        auto cursor = spellingAt(pos);
        if (!cursor.isNull())
            addSpellcheckActions(menu, cursor);
    
        menu->exec(_editor->mapToGlobal(pos));
        delete menu;
    }
    
    QTextCursor spellingAt(const QPoint& pos) const
    {
        auto cursor = _editor->cursorForPosition(_editor->viewport()->mapFromParent(pos));
        auto cursorPos = cursor.position();
        for (auto &es : _editor->extraSelections())
            if (cursorPos >= es.cursor.anchor() && cursorPos <= es.cursor.position())
                return es.cursor;
        return QTextCursor();
    }
    
    void addSpellcheckActions(QMenu* menu, QTextCursor& cursor)
    {
        auto word = cursor.selectedText();
    
        QList<QAction*> actions;
    
        auto variants = _spellchecker->suggest(word);
        if (variants.isEmpty())
        {
            auto actionNone = new QAction(tr("No variants"), menu);
            actionNone->setDisabled(true);
            actions << actionNone;
        }
        else
            for (auto variant : std::as_const(variants))
            {
                auto actionWord = new QAction(">  " + variant, menu);
                connect(actionWord, &QAction::triggered, [this, cursor, variant]{
                    _changesLocked = true;
                    const_cast<QTextCursor&>(cursor).insertText(variant);
                    _changesLocked = false;
                });
                actions << actionWord;
            }
    
        auto actionRemember = new QAction(tr("Add to dictionary"), menu);
        connect(actionRemember, &QAction::triggered, [this, cursor, word]{
            _spellchecker->save(word);
            _spellchecker->ignore(word);
        });
        actions << actionRemember;
    
        auto actionIgnore = new QAction(tr("Ignore this word"), menu);
        connect(actionIgnore, &QAction::triggered, [this, cursor, word]{
            _spellchecker->ignore(word);
        });
        actions << actionIgnore;
    
        auto actionSeparator = new QAction(menu);
        actionSeparator->setSeparator(true);
        actions << actionSeparator;
    
        auto existingActions = menu->actions();
        menu->insertActions(existingActions.first(), actions);
    }
};

//------------------------------------------------------------------------------
//                                Spellcheck
//------------------------------------------------------------------------------

Spellcheck::Spellcheck(QTextEdit *editor) : QObject(editor), _editor(editor)
{
}

Spellcheck::Spellcheck(QPlainTextEdit* editor) : QObject(editor), _editor(editor)
{
}

void Spellcheck::setLang(const QString &lang)
{
    if (lang == _lang) return;
    
    _lang = lang;
    
    if (_impl)
    {
        if (auto editor = qobject_cast<QTextEdit*>(_editor); editor)
        {
            auto impl = static_cast<SpellcheckImpl<QTextEdit>*>(_impl);
            impl->clearErrorMarks();
            delete impl;
            _impl = nullptr;
        }
        else if (auto editor = qobject_cast<QPlainTextEdit*>(_editor); editor)
        {
            auto impl = static_cast<SpellcheckImpl<QPlainTextEdit>*>(_impl);
            impl->clearErrorMarks();
            delete impl;
            _impl = nullptr;
        }
        else
            qWarning() << Q_FUNC_INFO << "Unsupported editor type";
    }
    
    if (!_lang.isEmpty())
    {
        auto engine = SpellcheckEngine::get(_lang);
        if (!engine) return; // Unable to open dictionary
        
        if (auto editor = qobject_cast<QTextEdit*>(_editor); editor)
        {
            auto impl = new SpellcheckImpl(editor, engine);
            impl->spellcheckAll();
            _impl = impl;
        }
        else if (auto editor = qobject_cast<QPlainTextEdit*>(_editor); editor)
        {
            auto impl = new SpellcheckImpl(editor, engine);
            impl->spellcheckAll();
            _impl = impl;
        }
        else
            qWarning() << Q_FUNC_INFO << "Unsupported editor type";
    }
}

//------------------------------------------------------------------------------
//                            SpellcheckerControl
//------------------------------------------------------------------------------

SpellcheckControl::SpellcheckControl(QObject* parent) : QObject(parent)
{
    auto dicts = SpellcheckEngine::dictionaries();
    if (dicts.isEmpty()) return;

    _actionGroup = new QActionGroup(parent);
    _actionGroup->setExclusive(true);
    connect(_actionGroup, &QActionGroup::triggered, this, &SpellcheckControl::actionGroupTriggered);

    auto actionNone = new QAction(tr("None"), this);
    actionNone->setCheckable(true);
    _actionGroup->addAction(actionNone);

    auto langNames = SpellcheckEngine::langNamesMap();
    for (auto& lang : dicts)
    {
        auto langName = langNames.contains(lang) ? langNames[lang] : lang;
        auto actionDict = new QAction(langName, this);
        actionDict->setCheckable(true);
        actionDict->setData(lang);
        _actionGroup->addAction(actionDict);
    }
}

QMenu* SpellcheckControl::makeMenu(QWidget* parent)
{
    if (!_actionGroup) return nullptr;

    auto menu = new QMenu(tr("Spellcheck"), parent);
    menu->addActions(_actionGroup->actions());
    return menu;
}

void SpellcheckControl::showCurrentLang(const QString& lang)
{
    if (!_actionGroup) return;

    for (auto action : _actionGroup->actions())
        if (action->data().toString() == lang)
        {
            action->setChecked(true);
            break;
        }
}

void SpellcheckControl::setEnabled(bool on)
{
    if (_actionGroup) _actionGroup->setEnabled(on);
}

void SpellcheckControl::actionGroupTriggered(QAction* action)
{
    emit langSelected(action->data().toString());
}

} // namespace Ori
