#include "OriTableWidgetBase.h"

#include <QBoxLayout>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QTableView>

namespace Ori {

TableWidgetBase::TableWidgetBase(QWidget *parent) : QWidget(parent)
{
    columnIndexId = 0;
    columnIndexTitle = 1;
    _tableModel = NULL;
    tableView = NULL;
    itemDelegate = NULL;

    mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);
}

TableWidgetBase::~TableWidgetBase()
{
    reset();

    if (itemDelegate == NULL)
        delete itemDelegate;
}

void TableWidgetBase::update()
{
    reset();

    _tableModel = createTableModel();

    QMapIterator<int, QString> header(columnHeaders);
    while (header.hasNext())
    {
        header.next();
        _tableModel->setHeaderData(header.key(), Qt::Horizontal, header.value());
    }

    tableView = new QTableView;
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSortingEnabled(false);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->verticalHeader()->hide();
    tableView->setAlternatingRowColors(true);
    tableView->setModel(_tableModel);
    if (itemDelegate)
        tableView->setItemDelegate(itemDelegate);

    updateHiddenColumns();

    QFont font = tableView->font();
    font.setPointSize(10);
    tableView->setFont(font);

    adjustHeader();

    connect(tableView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(tableViewDoubleClicked(QModelIndex)));
    connect(tableView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(tableViewContextMenuRequested(QPoint)));

    mainLayout->addWidget(tableView);
    tableView->setFocus();

    tableCreated();
}

void TableWidgetBase::updateHiddenColumns()
{
    for (int i = 0; i < hiddenColumns.size(); i++)
        tableView->setColumnHidden(hiddenColumns.at(i), true);
}

void TableWidgetBase::reset()
{
    if (tableView != NULL)
    {
        mainLayout->removeWidget(tableView);
        delete tableView;
        tableView = NULL;
    }
    if (_tableModel != NULL)
    {
        delete _tableModel;
        _tableModel = NULL;
    }
}

void TableWidgetBase::setSelectedRow(int row)
{
    if (!tableView) return;
    tableView->selectRow(row);
    tableView->setFocus();
    adjustHeader();
}

void TableWidgetBase::setSelectedId(int id)
{
    if (!_tableModel or !tableView) return;
    QModelIndexList indexes = _tableModel->match(
            _tableModel->index(0, columnIndexId), Qt::DisplayRole,
            QVariant::fromValue(id), 1, Qt::MatchExactly);
    if (!indexes.empty())
        tableView->selectRow(indexes.at(0).row());
}

QString TableWidgetBase::selectedTitle()
{
    int row = selectedRow();
    if (row > -1)
        return _tableModel->data(_tableModel->index(row, columnIndexTitle)).toString();
    return QString();
}

int TableWidgetBase::selectedId()
{
    int row = selectedRow();
    if (row > -1)
    {
        return _tableModel->data(_tableModel->index(row, columnIndexId)).toInt();
    }
    return -1;
}

int TableWidgetBase::selectedRow()
{
    if (_tableModel)
    {
        QModelIndexList selection = tableView->selectionModel()->selectedRows();
        if (!selection.empty())
            return selection.at(0).row();
    }
    return -1;
}

void TableWidgetBase::tableViewDoubleClicked(const QModelIndex&)
{
    emit onDoubleClick();
}

void TableWidgetBase::tableViewContextMenuRequested(const QPoint& pos)
{
    emit onContextMenu(tableView->mapToGlobal(pos));
}

} // namespace Ori
