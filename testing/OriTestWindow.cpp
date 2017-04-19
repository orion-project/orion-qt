#ifndef ORI_TEST_WINDOW_H
#define ORI_TEST_WINDOW_H

#include "OriTestWindow.h"
#include "../tools/OriSettings.h"
#include "../tools/OriTranslator.h"
#include "../helpers/OriWidgets.h"
#include "../widgets/OriLangsMenu.h"

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QShowEvent>
#include <QSplitter>
#include <QStatusBar>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>

namespace Ori {
namespace Test {

////////////////////////////////////////////////////////////////////////////////

TestWindow::TestWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(qApp->applicationName() + " Tests");

    Ori::Settings s;
    s.restoreWindowGeometry("testWindow", this, QSize(640, 640));

    s.beginGroup("View");
    _translator = new Ori::Translator(s.strValue("language"), this);

    testsTree = Ori::Gui::twoColumnTree(tr("Test"), tr("Message"));
    connect(testsTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(testSelected(QTreeWidgetItem*,QTreeWidgetItem*)));

    auto actionRunAll = Ori::Gui::action(tr("Run &All"), this, SLOT(runAll()), ":/toolbar/run_tests");
    auto actionRunSelected = Ori::Gui::action(tr("Run &Selected"), this, SLOT(runSelected()), ":/toolbar/run_test");
    auto actionResetState = Ori::Gui::action(tr("&Reset All"), this, SLOT(resetState()), ":/toolbar/reset_tests");
    auto actionCollapseTree = Ori::Gui::action(tr("&Collapse All"), testsTree, SLOT(collapseAll()), ":/toolbar/collapse_tests");
    auto actionExpandTree = Ori::Gui::action(tr("&Expand All"), testsTree, SLOT(expandAll()), ":/toolbar/expand_tests");
    actionSaveLog = Ori::Gui::toggledAction(tr("Save Results to Log File"), this, 0);

    addToolBar(Qt::TopToolBarArea, Ori::Gui::toolbar({
        Ori::Gui::textToolButton(actionRunAll),
        Ori::Gui::textToolButton(actionRunSelected),
        0, actionResetState, actionExpandTree, actionCollapseTree }));

    menuBar()->addMenu(Ori::Gui::menu(tr("&Test"), { actionRunAll, actionRunSelected, 0,
        actionResetState, 0, actionExpandTree, actionCollapseTree }));
    menuBar()->addMenu(new Ori::Widgets::LanguagesMenu(_translator));
    menuBar()->addMenu(Ori::Gui::menu(tr("&Options"), { actionSaveLog }));

    progress = new QProgressBar;
    progress->setMaximumHeight(10);
    progress->setVisible(false);
    testsTotal = 0;
    labelTotal = new QLabel;
    labelRun = new QLabel;
    labelPass = new QLabel;
    labelFail = new QLabel;
    setStatusInfo(CountTotal, 0);
    setStatusInfo(CountRun, 0);
    setStatusInfo(CountPass, 0);
    setStatusInfo(CountFail, 0);
    statusBar()->addWidget(labelTotal);
    statusBar()->addWidget(labelRun);
    statusBar()->addWidget(labelPass);
    statusBar()->addWidget(labelFail);
    statusBar()->addWidget(progress);
    statusBar()->setVisible(true);

    hideProgressTimer = new QTimer(this);
    hideProgressTimer->setSingleShot(true);
    connect(hideProgressTimer, SIGNAL(timeout()), progress, SLOT(hide()));

    testLog = Ori::Gui::logView();

    auto splitter = Ori::Gui::splitterV(testsTree, testLog);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);
}

TestWindow::~TestWindow()
{
    Ori::Settings s;
    s.beginGroup("View");
    s.setValue("language", _translator->currentLanguage());

    s.beginGroup("TestsExpanded");
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* it = testsTree->topLevelItem(i);
        s.setValue(it->text(0), it->isExpanded());
        saveExpandedStates(it, it->text(0), s);
    }

    s.storeWindowGeometry("testWindow", this);
}

void TestWindow::saveExpandedStates(QTreeWidgetItem* root, const QString& rootPath, Ori::Settings& settings)
{
    for (int i = 0; i < root->childCount(); i++)
    {
        auto item = root->child(i);
        if (item->childCount() > 0)
        {
            auto path = QString("%1/%2").arg(rootPath, item->text(0));
            settings.setValue(path, item->isExpanded());
            saveExpandedStates(item, path, settings);
        }
    }
}

TestGroup* getGroupByName(const TestSuite &tests, const QString& name)
{
    for (auto test: tests)
    {
        auto group = asGroup(test);
        if (group && group->name() == name)
            return group;
    }
    return nullptr;
}

TestSuite mergeGroupsByName(const TestSuite &tests)
{
    TestSuite results;
    for (auto test: tests)
    {
        auto group = asGroup(test);
        if (!group)
        {
            results.append(test);
            continue;
        }
        auto existedGroup = getGroupByName(results, group->name());
        if (!existedGroup)
        {
            results.append(group);
            continue;
        }
        for (auto test: group->tests())
            existedGroup->append(test);
    }
    return results;
}

void TestWindow::setTests(const TestSuite &tests)
{
    testsTotal = 0;
    setTests(nullptr, mergeGroupsByName(tests));
    setStatusInfo(CountTotal, testsTotal);

    Ori::Settings s;
    s.beginGroup("TestsExpanded");
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* it = testsTree->topLevelItem(i);
        it->setExpanded(s.value(it->text(0), true).toBool());
        loadExpandedStates(it, it->text(0), s);
    }
}

void TestWindow::loadExpandedStates(QTreeWidgetItem* root, const QString& rootPath, Ori::Settings& settings)
{
    for (int i = 0; i < root->childCount(); i++)
    {
        auto item = root->child(i);
        if (item->childCount() > 0)
        {
            auto path = QString("%1/%2").arg(rootPath, item->text(0));
            item->setExpanded(settings.value(path, true).toBool());
            loadExpandedStates(item, path, settings);
        }
    }
}

void TestWindow::setTests(QTreeWidgetItem *root, const TestSuite &tests)
{
    foreach (TestBase *test, tests)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setData(0, Qt::UserRole, QVariant::fromValue((void*)test));
        item->setText(0, test->name());

        TestGroup *group = dynamic_cast<TestGroup*>(test);
        if (group)
        {
            setTests(item, group->tests());

            QFont font = item->font(0);
            font.setBold(true);
            item->setFont(0, font);
        }
        else
            testsTotal++;

        resetState(item);

        if (root)
            root->addChild(item);
        else
            testsTree->addTopLevelItem(item);
    }
}

void TestWindow::resetState()
{
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
        resetState(testsTree->topLevelItem(i));
}

void TestWindow::resetState(QTreeWidgetItem *root)
{
    static QIcon iconTestUnknown = QIcon(":/tests/states/undef");

    root->setIcon(0, iconTestUnknown);
    root->setText(1, QString());

    for (int i = 0; i < root->childCount(); i++)
    {
        QTreeWidgetItem *child = root->child(i);
        if (child->childCount() == 0)
        {
            child->setIcon(0, iconTestUnknown);
            child->setText(1, QString());
        }
        else resetState(child);
    }
}

void TestWindow::setState(QTreeWidgetItem *item, bool success)
{
    static QIcon iconTestSuccess = QIcon(":/tests/states/pass");
    static QIcon iconTestFail = QIcon(":/tests/states/fail");
    item->setIcon(0, success? iconTestSuccess: iconTestFail);
}

void TestWindow::setStatusInfo(StatusInfoKind kind, int value)
{
    switch (kind)
    {
    case CountTotal:
        labelTotal->setText(QString("   %1: %2   ").arg(tr("Total")).arg(value));
        break;

    case CountRun:
        labelRun->setText(QString("   %1: %2   ").arg(tr("Run")).arg(value));
        break;

    case CountPass:
        labelPass->setText(QString("   %1: %2   ").arg(tr("Successful")).arg(value));
        break;

    case CountFail:
        labelFail->setText(QString("   %1: %2   ").arg(tr("Failed")).arg(value));
        break;
    }
}

void TestWindow::resetStatistics()
{
    setStatusInfo(CountRun, 0);
    setStatusInfo(CountPass, 0);
    setStatusInfo(CountFail, 0);
}

TestBase* TestWindow::getTest(QTreeWidgetItem *item)
{
    return (TestBase*)(item->data(0, Qt::UserRole).value<void*>());
}

void TestWindow::runAll()
{
    TestLogger::enable(actionSaveLog->isChecked());

    progress->setValue(0);
    progress->setMaximum(testsTotal);
    progress->setVisible(true);

    resetStatistics();
    resetState();

    TestSession session;
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
        runTest(testsTree->topLevelItem(i), session);

    hideProgressTimer->start(500);
}

void TestWindow::runSelected()
{
    TestLogger::enable(actionSaveLog->isChecked());

    QList<QTreeWidgetItem*> selected = testsTree->selectedItems();
    if (selected.count() > 0)
    {
        int tests_count = testsCount(selected[0]);
        if (tests_count > 1)
        {
            progress->setValue(0);
            progress->setMaximum(tests_count);
            progress->setVisible(true);
        }

        resetStatistics();
        resetState(selected[0]);

        TestSession session;
        runTest(selected[0], session);

        if (tests_count > 1)
            hideProgressTimer->start(500);
    }
}

void TestWindow::runTest(QTreeWidgetItem *item, TestSession &session)
{
    TestBase *test = getTest(item);
    test->reset();

    if (item->childCount() > 0) // it is a group
    {
        for (int i = 0; i < item->childCount(); i++)
            runTest(item->child(i), session);
    }
    else
    {
        static QIcon iconTestRun = QIcon(":/tests/states/run");
        item->setIcon(0, iconTestRun);
        QApplication::processEvents();

        session.run(test);

        progress->setValue(progress->value()+1);
    }

    item->setText(1, test->message());
    setState(item, test->result());
    setStatusInfo(CountRun, session.testsRun());
    setStatusInfo(CountPass, session.testsPass());
    setStatusInfo(CountFail, session.testsFail());
    QApplication::processEvents();

    testLog->setText(test->log());
}

void TestWindow::testSelected(QTreeWidgetItem *selected, QTreeWidgetItem*)
{
    if (!selected) return;

    TestBase *test = getTest(selected);
    testLog->setText(test->log());
}

int TestWindow::testsCount(QTreeWidgetItem *item)
{
    if (item->childCount() > 0)
    {
        int result = 0;
        for (int i = 0; i < item->childCount(); i++)
        {
            if (item->child(i)->childCount() > 0)
                result += testsCount(item->child(i));
            else
                result++;
        }
        return result;
    }
    return 1;
}

} // namespace Test
} // namespace Ori

#endif // ORI_TEST_WINDOW_H
