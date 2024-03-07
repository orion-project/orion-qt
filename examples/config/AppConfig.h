#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "core/OriTemplates.h"

#include <QObject>

/**
    Example configuration object
*/
class AppConfig : public QObject, public Ori::Singleton<AppConfig>
{
public:
    bool showProtocolAtStart; ///< Open protocol window just after application started.
    bool useOnlineHelp;       ///< Navigate to online help instead of opening Assistant

    bool showBackground;      ///< Show background image in main window.
    bool useSystemDialogs;    ///< Use native open/save file dialogs.

    void load();
    void save();

    enum { pageGeneral, pageInterface };
    bool edit(int currentPageId = -1);
};

#endif // APP_CONFIG_H
