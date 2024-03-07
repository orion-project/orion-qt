#include "AppConfig.h"

#include "dialogs/OriConfigDlg.h"
#include "helpers/OriDialogs.h"
#include "tools/OriSettings.h"

#include <QMessageBox>

using namespace Ori::Dlg;

#define LOAD(option, type, default_value)\
    option = s.settings()->value(QStringLiteral(#option), default_value).to ## type()

#define SAVE(option)\
    s.settings()->setValue(QStringLiteral(#option), option)

void AppConfig::load()
{
    Ori::Settings s;

    s.beginGroup("General");
    LOAD(showProtocolAtStart, Bool, false);
    LOAD(useOnlineHelp, Bool, false);

    s.beginGroup("View");
    LOAD(showBackground, Bool, true);
    LOAD(useSystemDialogs, Bool, true);
}

void AppConfig::save()
{
    Ori::Settings s;

    s.beginGroup("General");
    SAVE(showProtocolAtStart);
    SAVE(useOnlineHelp);

    s.beginGroup("View");
    SAVE(showBackground);
    SAVE(useSystemDialogs);
}

bool AppConfig::edit(int currentPageId)
{
    ConfigDlgOpts opts;
    opts.objectName = "ConfigDlg";
    opts.currentPageId = currentPageId;
    opts.pages = {
        ConfigPage(pageGeneral, tr("General"), ":/ori_images/info"),
        ConfigPage(pageInterface, tr("Interface")).withHelpTopic("help-config-interface"),
    };
    opts.items = {
        new ConfigItemBool(pageGeneral, tr("Open protocol window just after application started"), &showProtocolAtStart),
        new ConfigItemBool(pageGeneral, tr("Navigate to online help instead of opening Assistant"), &useOnlineHelp),
        new ConfigItemBool(pageInterface, tr("Show background image in main window"), &showBackground),
        new ConfigItemBool(pageInterface, tr("Use native open/save file dialogs"), &useSystemDialogs),
    };
    opts.onHelpRequested = [](const QString& topic){ Ori::Dlg::info("Help requested: " + topic); };
    if (ConfigDlg::edit(opts))
    {
        save();
        return true;
    }
    return false;
}
