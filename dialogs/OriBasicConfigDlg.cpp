#include "OriBasicConfigDlg.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>

// TODO configurable icon size
#define PAGE_LIST_ICON_SIZE 24
#define PAGE_LIST_ITEM_SIZE 30

namespace Ori {
namespace Dlg {

struct ConfigDialogState
{
    int currentPageIndex;
};

QMap<QString, ConfigDialogState> __savedConfigDialogStates;

//------------------------------------------------------------------------------
//                             BasicConfigDialog
//------------------------------------------------------------------------------

BasicConfigDialog::BasicConfigDialog(QWidget* parent) : QDialog(parent)
{
    // pages
    pageView = new QStackedWidget;

    pageList = new QListWidget();
    pageList->setIconSize(QSize(PAGE_LIST_ICON_SIZE, PAGE_LIST_ICON_SIZE));
    connect(pageList, SIGNAL(currentRowChanged(int)), this, SLOT(pageListItemSelected(int)));

    // header
    pageHeader = new QLabel();
    QFont fnt = this->font();
    fnt.setPointSize(fnt.pointSize() + 2);
    fnt.setBold(true);
    pageHeader->setFont(fnt);

    // placement
    QVBoxLayout* layoutPage = new QVBoxLayout();
    layoutPage->addWidget(pageHeader);
    layoutPage->addWidget(pageView);

    QHBoxLayout* layoutPages = new QHBoxLayout;
    layoutPages->addWidget(pageList);
    layoutPages->addSpacing(qApp->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing));
    layoutPages->addLayout(layoutPage);

    // dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(showHelp()));
    helpButton = (QWidget*)buttonBox->button(QDialogButtonBox::Help);

    // main container
    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addLayout(layoutPages);
    layoutMain->addWidget(buttonBox);
    setLayout(layoutMain);
}

BasicConfigDialog::~BasicConfigDialog()
{
    storeState();
}

void BasicConfigDialog::createPages(QList<QWidget*> pages)
{
    QSize itemSize(0, PAGE_LIST_ITEM_SIZE);
    for (int i = 0; i < pages.length(); i++)
    {
        QWidget *page = pages.at(i);
        pageList->addItem(page->windowTitle());
        pageList->item(i)->setIcon(page->windowIcon());
        pageList->item(i)->setSizeHint(itemSize);
        pageView->addWidget(page);
    }

    adjustPageList();
    adjustHelpButton();

    if (pageList->count() > 0)
    {
        if (!restoreState())
            setCurrentPageIndex(0);

        populate();
    }
}

void BasicConfigDialog::storeState()
{
    if (objectName().isEmpty()) return;
    ConfigDialogState state = {};
    state.currentPageIndex = currentPageIndex();
    __savedConfigDialogStates[objectName()] = state;
}

bool BasicConfigDialog::restoreState()
{
    QString objectName = this->objectName();
    if (objectName.isEmpty()) return false;
    if (!__savedConfigDialogStates.contains(objectName)) return false;
    int savedPageIndex = __savedConfigDialogStates[objectName].currentPageIndex;
    if (savedPageIndex < 0 && savedPageIndex >= pageList->count()-1) return false;
    setCurrentPageIndex(savedPageIndex);
    return true;
}

/// Calculate a sensible width for the items list.
void BasicConfigDialog::adjustPageList()
{
    QFontMetrics fm(pageList->font());
    int width, max_width = 0;
    for (int i = 0; i < pageList->count(); i++)
    {
        QStringList strs = pageList->item(i)->text().split('\n');
        for (int j = 0; j < strs.count(); j++)
        {
            width = fm.width(strs[j]);
            if(width > max_width) max_width = width;
        }
    }
    QStyle* style = qApp->style();
    max_width += PAGE_LIST_ICON_SIZE +
                 style->pixelMetric(QStyle::PM_ScrollBarExtent) +
                 style->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2 +
                 style->pixelMetric(QStyle::PM_LayoutLeftMargin) +
                 style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing) +
                 style->pixelMetric(QStyle::PM_LayoutRightMargin);
    pageList->setMaximumWidth(max_width);
    pageList->setMinimumWidth(max_width);
}

/// Hide help button if no one page has a help topic assigned.
void BasicConfigDialog::adjustHelpButton()
{
    for (int i = 0; i < pageList->count(); i++)
    {
        BasicConfigPage* page = dynamic_cast<BasicConfigPage*>(pageView->widget(i));
        if (page && !page->helpTopic().isEmpty()) return;
    }
    helpButton->setVisible(false);
}

int BasicConfigDialog::currentPageIndex() const
{
    return pageList->currentRow();
}

void BasicConfigDialog::setCurrentPageIndex(int index)
{
    pageList->setCurrentRow(index);
}

void BasicConfigDialog::pageListItemSelected(int index)
{
    pageView->setCurrentIndex(index);

    if (pageList->currentItem())
        pageHeader->setText(pageList->currentItem()->text().replace('\n', ' '));

    helpButton->setEnabled(!currentHelpTopic().isEmpty());
}

void BasicConfigDialog::accept()
{
    if (!collect()) return;
    QDialog::accept();
}

void BasicConfigDialog::showHelp()
{
    // TODO help system
    qWarning() << "Page help topic:" << currentHelpTopic();
}

QString BasicConfigDialog::currentHelpTopic() const
{
    BasicConfigPage* page = dynamic_cast<BasicConfigPage*>(pageView->currentWidget());
    return page? page->helpTopic(): QString();
}

//------------------------------------------------------------------------------
//                             BasicConfigPage
//------------------------------------------------------------------------------

BasicConfigPage::BasicConfigPage(const QString& title,
                                 const QString& iconPath,
                                 const QString& helpTopic) : QWidget()
{
    setWindowTitle(title);
    setWindowIcon(QIcon(iconPath));

    _helpTopic = helpTopic;

    _mainLayout = new QVBoxLayout(this);
    _mainLayout->setMargin(0);
}

void BasicConfigPage::add(std::initializer_list<QObject*> items)
{
    for (auto item: items)
    {
        auto widget = qobject_cast<QWidget*>(item);
        if (widget)
        {
            _mainLayout->addWidget(widget);
            continue;
        }
        auto layout = qobject_cast<QLayout*>(item);
        if (layout)
        {
            _mainLayout->addLayout(layout);
            continue;
        }
    }
}

} // namespace Dlg
} // namespace Ori
