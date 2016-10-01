#include <QDebug>
#include <QFileDialog>
#include <QMenuBar>
#include <QMainWindow>
#include <QMessageBox>

#include "../../tools/OriMruList.h"
#include "../../helpers/OriWidgets.h"
#include "../../widgets/OriMruMenu.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow()
    {
        // MRU-list contains list of some generalized items.
        // MRU-file-list contains list of recently used files.
        // Is differs from generalized MRU-list in that it checks if file exists
        // and compares items as file paths in operation system specific way.
        mru = new Ori::MruFileList(this);
        connect(mru, SIGNAL(clicked(QString)), this, SLOT(mruItemClicked(QString)));

        // If we load MRU-list, it will automatically save its items when changed
        mru->load();

        // Widget displaying a content of MRU-list
        auto mruWidget = new Ori::Widgets::MruFileListWidget(mru);

        // You can override default header
        mruWidget->setHeader("Recently Used Files:");

        // MRU-menu is used to display MRU-list as submenu
        auto mruMenu = new Ori::Widgets::MruMenu(mru);

        auto actionOpen = Ori::Gui::action("Open File...", this, SLOT(openFile()));
        auto actionClose = Ori::Gui::action("Close", this, SLOT(close()));
        auto menuFile = Ori::Gui::menu("File", {
            actionOpen,
            0,
            mruMenu,
            0,
            actionClose
        });

        // MRU-menu-part is used to built MRU-lits into existed menu.
        // MRU-items are inserted into the menu just before the 'placeholder' action
        // and are delimited from that by menu separator.
        new Ori::Widgets::MruMenuPart(mru, menuFile, actionClose, this);

        menuBar()->addMenu(menuFile);
        setCentralWidget(mruWidget);
        resize(800, 600);
    }

private slots:
    void openFile()
    {
        mru->append(QFileDialog::getOpenFileName());
    }

    void mruItemClicked(const QString& item)
    {
        QMessageBox::information(this, "MRU item clicked", item);
    }

private:
    Ori::MruFileList* mru;
};
