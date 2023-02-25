#include "OriBasicConfigDlg.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>

namespace Ori {
namespace Dlg {

struct ConfigDialogState
{
    int currentPageIndex;
    QByteArray geometry;
};

namespace {
QMap<QString, ConfigDialogState> __savedConfigDialogStates;
}

//------------------------------------------------------------------------------
//                             BasicConfigDialog
//------------------------------------------------------------------------------

BasicConfigDialog::BasicConfigDialog(QWidget* parent) : QDialog(parent)
{
    // pages
    pageView = new QStackedWidget;

    pageList = new QListWidget();
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
    helpButton = qobject_cast<QWidget*>(buttonBox->button(QDialogButtonBox::Help));

    // main container
    QVBoxLayout *layoutMain = new QVBoxLayout;
#ifdef Q_OS_MAC
    layoutMain->setSpacing(12);
#endif
    layoutMain->addLayout(layoutPages);
#ifdef Q_OS_WIN
    layoutMain->addSpacing(6);
#endif
    layoutMain->addWidget(buttonBox);
    setLayout(layoutMain);
}

BasicConfigDialog::~BasicConfigDialog()
{
    storeState();
}

void BasicConfigDialog::setTitleAndIcon(const QString& title, const QString& iconPath)
{
    setWindowTitle(title);

#ifndef Q_OS_MACOS
    setWindowIcon(QIcon(iconPath));
#else
    // On MacOS dialog icon overrides app icon in the dock and it looks ugly.
    Q_UNUSED(iconPath)
#endif
}

void BasicConfigDialog::createPages(QList<QWidget*> pages)
{
    pageList->setIconSize(pageListIconSize);
    QSize itemSize(0, pageListIconSize.height() + 2 * pageListSpacing);
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
    state.geometry = saveGeometry();
    __savedConfigDialogStates[objectName()] = state;
}

bool BasicConfigDialog::restoreState()
{
    QString objectName = this->objectName();
    if (objectName.isEmpty()) return false;
    if (!__savedConfigDialogStates.contains(objectName)) return false;
    auto state = __savedConfigDialogStates[objectName];
    int savedPageIndex = state.currentPageIndex;
    if (savedPageIndex < 0 && savedPageIndex >= pageList->count()-1) return false;
    setCurrentPageIndex(savedPageIndex);
    if (!state.geometry.isEmpty())
        restoreGeometry(state.geometry);
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
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
            width = fm.horizontalAdvance(strs[j]);
        #else
            width = fm.width(strs[j]);
        #endif
            if(width > max_width) max_width = width;
        }
    }
    QStyle* style = qApp->style();
    max_width += pageListIconSize.width() +
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

    if (helpTopic().isEmpty())
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

PageId BasicConfigDialog::currentPageId() const
{
    int idx = currentPageIndex();
    auto page = qobject_cast<BasicConfigPage*>(pageView->widget(idx));
    return page ? page->id() : PageId();
}

void BasicConfigDialog::setCurrentPageId(const PageId& id)
{
    if (!id.set) return;
    for (int i = 0; i < pageView->count(); i++)
    {
        auto page = qobject_cast<BasicConfigPage*>(pageView->widget(i));
        if (page && page->id() == id)
        {
            setCurrentPageIndex(i);
            return;
        }
    }
}

void BasicConfigDialog::pageListItemSelected(int index)
{
    pageView->setCurrentIndex(index);

    BasicConfigPage* page = currentPage();
    if (page && !page->longTitle().isEmpty())
        pageHeader->setText(page->longTitle());
    else if (pageList->currentItem())
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
    auto topic = currentHelpTopic();
    if (!topic.isEmpty())
        emit helpRequested(topic);
}

BasicConfigPage* BasicConfigDialog::currentPage() const
{
    return dynamic_cast<BasicConfigPage*>(pageView->currentWidget());
}

QString BasicConfigDialog::currentHelpTopic() const
{
    BasicConfigPage* page = currentPage();
    return page? page->helpTopic(): helpTopic();
}

//------------------------------------------------------------------------------
//                             BasicConfigPage
//------------------------------------------------------------------------------

BasicConfigPage::BasicConfigPage(const QString& title,
                                 const QString& iconPath,
                                 const QString& helpTopic)
    : BasicConfigPage(PageId(), title, iconPath, helpTopic)
{}

BasicConfigPage::BasicConfigPage(PageId id,
                                 const QString& title,
                                 const QString& iconPath,
                                 const QString& helpTopic) : QWidget()
{
    setWindowTitle(title);
    setWindowIcon(QIcon(iconPath));

    _id = id;
    _helpTopic = helpTopic;

    _mainLayout = new QVBoxLayout(this);
    _mainLayout->setContentsMargins(0, 0, 0, 0);
}

void BasicConfigPage::add(std::initializer_list<QObject*> items)
{
    for (auto item: items)
    {
        if (item == &_stretchDummy)
        {
            _mainLayout->addStretch();
            continue;
        }
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
