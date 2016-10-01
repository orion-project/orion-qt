#include "OriLangsMenu.h"
#include "../tools/OriTranslator.h"

#include <QApplication>
#include <QMessageBox>

namespace Ori {
namespace Widgets {

LanguagesMenu::LanguagesMenu(Translator* translator, QWidget *parent) : LanguagesMenu(translator, QString(), parent)
{
}

LanguagesMenu::LanguagesMenu(Translator *translator, const QString& icon, QWidget *parent) : QMenu(parent), _translator(translator)
{
    setTitle(tr("&Language", "Menu title"));

    if (!icon.isEmpty())
        setIcon(QPixmap(icon));

    connect(this, SIGNAL(aboutToShow()), this, SLOT(hightlightCurrentLanguage()));
}

void LanguagesMenu::populate()
{
    for (const QString& lang: _translator->getLanguages())
    {
        auto a = addAction(_translator->getLanguageName(lang), this, SLOT(applyLanguage()));
        a->setData(lang);
        a->setCheckable(true);
    }
}

void LanguagesMenu::applyLanguage()
{
    _translator->setCurrentLanguage(qobject_cast<QAction*>(sender())->data().toString());

    if (!_translator->isApplyImmediately())
        QMessageBox::information(qApp->activeWindow(), qApp->applicationName(),
            QString::fromLatin1("Language will be applied after application restart"));
}

void LanguagesMenu::hightlightCurrentLanguage()
{
    if (isEmpty()) populate();

    auto lang = _translator->currentLanguage();
    for (auto a: actions())
        a->setChecked(a->data().toString() == lang);
}

} // namespace Widgets
} // namespace Ori
