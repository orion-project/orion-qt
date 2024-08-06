#ifndef ORI_HELP_WINDOW_H
#define ORI_HELP_WINDOW_H

#include <QWidget>

// See `$ORI/examples/help` for usage exmaple

namespace Ori {

class HelpBrowser;

class HelpWindow : public QWidget
{
    Q_OBJECT

public:
    static void showContent();
    static void showTopic(const QString& topic);

    static bool isDevMode;
    static std::function<QString()> getCssSrc;
    static std::function<QString()> getHelpDir;

private:
    static void openWindow();

    explicit HelpWindow();
    ~HelpWindow();

    HelpBrowser *_browser;

    void editStyleSheet();
};

} // namespace Ori

#endif // ORI_HELP_WINDOW_H
