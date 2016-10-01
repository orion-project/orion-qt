#ifndef ORI_TABLE_WIDGET_BASE_H
#define ORI_TABLE_WIDGET_BASE_H

#include <QWidget>
#include <QMap>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QModelIndex;
class QStyledItemDelegate;
class QTableView;
class QVBoxLayout;
QT_END_NAMESPACE

namespace Ori {

class TableWidgetBase : public QWidget
{
    Q_OBJECT

public:
    explicit TableWidgetBase(QWidget *parent = 0);
    ~TableWidgetBase();

    void reset();
    void update();
    QAbstractItemModel* tableModel() { return _tableModel; }
    void setSelectedRow(int row);
    void setSelectedId(int id);
    int selectedRow();
    int selectedId();
    QString selectedTitle();
    virtual void adjustHeader() {}

signals:
    void onDoubleClick();
    void onContextMenu(const QPoint&);

protected:
    QAbstractItemModel *_tableModel;
    QStyledItemDelegate *itemDelegate;
    QTableView *tableView;
    QVBoxLayout *mainLayout;
    QMap<int, QString> columnHeaders;
    QList<int> hiddenColumns;
    int columnIndexId;
    int columnIndexTitle;

    virtual QAbstractItemModel* createTableModel() = 0;
    virtual void tableCreated() {}
    void updateHiddenColumns();

private slots:
    void tableViewDoubleClicked(const QModelIndex&);
    void tableViewContextMenuRequested(const QPoint&);
};

} // namespace Ori

#endif // ORI_TABLE_WIDGET_BASE_H
