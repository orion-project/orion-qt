#ifndef ORI_BASIC_CONFIG_DLG_H
#define ORI_BASIC_CONFIG_DLG_H

#include <QDialog>

#include "core/OriTemplates.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QListWidget;
class QStackedWidget;
class QLabel;
QT_END_NAMESPACE

namespace Ori {
namespace Dlg {

class BasicConfigPage;

using PageId = Ori::Optional<int>;

class BasicConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BasicConfigDialog(QWidget* parent = nullptr);
    ~BasicConfigDialog();

    virtual void populate() {}
    virtual bool collect() { return true; }

    bool run() { return exec() == QDialog::Accepted; }

    int currentPageIndex() const;
    void setCurrentPageIndex(int index);
    PageId currentPageId() const;
    void setCurrentPageId(const PageId &id);
    BasicConfigPage* currentPage() const;

signals:
    void helpRequested(const QString& topic);

protected:
    QSize pageListIconSize = QSize(24, 24);
    int pageListSpacing = 3;

    void setTitleAndIcon(const QString& title, const QString& iconPath);
    void createPages(QList<QWidget*>);

    /// Returns help topic for the whole dialog.
    /// It is used when there is no help topic for current page available.
    virtual QString helpTopic() const { return QString(); }

protected slots:
    void pageListItemSelected(int index);
    void showHelp();
    void accept();

private:
    QListWidget* pageList;
    QStackedWidget* pageView;
    QLabel* pageHeader;
    QWidget* helpButton;

    void adjustPageList();
    void adjustHelpButton();
    QString currentHelpTopic() const;
    void storeState();
    bool restoreState();
};

class BasicConfigPage : public QWidget
{
    Q_OBJECT

public:
    explicit BasicConfigPage(const QString& title,
                             const QString& iconPath = QString(),
                             const QString& helpTopic = QString());

    explicit BasicConfigPage(PageId id,
                             const QString& title,
                             const QString& iconPath = QString(),
                             const QString& helpTopic = QString());

    const PageId& id() const { return _id; }
    const QString& helpTopic() const { return _helpTopic; }

    void setLongTitle(const QString& title) { _longTitle = title; }
    const QString& longTitle() const { return _longTitle; }

    void add(std::initializer_list<QObject*> items);
    void add(QObject* item);
    QVBoxLayout* mainLayout() { return _mainLayout; }
    QObject* stretch() { return &_stretchDummy; }

private:
    PageId _id = PageId();
    QString _helpTopic, _longTitle;
    QVBoxLayout* _mainLayout;
    QObject _stretchDummy;
};

} // namespace Dlg
} // namespace Ori

#endif // ORI_BASIC_CONFIG_DLG_H
