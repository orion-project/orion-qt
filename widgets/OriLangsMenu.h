#ifndef ORI_LANGS_MENU_H
#define ORI_LANGS_MENU_H

#include <QMenu>

namespace Ori {

class Translator;

namespace Widgets {

class LanguagesMenu : public QMenu
{
    Q_OBJECT

public:
    explicit LanguagesMenu(Translator* translator, QWidget *parent = 0);
    explicit LanguagesMenu(Translator* translator, const QString& icon, QWidget *parent = 0);

private slots:
    void applyLanguage();
    void hightlightCurrentLanguage();

private:
    Translator* _translator;

    void populate();
};

} // namespace Widgets
} // namespace Ori

#endif // ORI_LANGS_MENU_H
