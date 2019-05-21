#include "OriTestWindow.h"

#include "../tools/OriSettings.h"
#include "../helpers/OriWidgets.h"
#include "../widgets/OriFlatToolBar.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QShowEvent>
#include <QSplitter>
#include <QStatusBar>
#include <QPlainTextEdit>
#include <QThread>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>

namespace Ori {
namespace Testing {

TestWindow::TestWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(qApp->applicationName() + " Tests");
    setWindowIcon(QIcon(":/ori_test_window/icon"));

    Ori::Settings s;
    s.restoreWindowGeometry("testWindow", this, QSize(640, 640));

    testsTree = Ori::Gui::twoColumnTree(tr("Test"), tr("Message"));
    connect(testsTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(showItemLog(QTreeWidgetItem*)));

    _actionRunAll = Ori::Gui::action(tr("Run &All"), this, SLOT(runAll()), ":/ori_test_window/run_tests");
    _actionRunSelected = Ori::Gui::action(tr("Run &Selected"), this, SLOT(runSelected()), ":/ori_test_window/run_test");
    _actionResetState = Ori::Gui::action(tr("&Reset All"), this, SLOT(resetState()), ":/ori_test_window/reset_tests");
    auto actionCollapseTree = Ori::Gui::action(tr("&Collapse All"), testsTree, SLOT(collapseAll()), ":/ori_test_window/collapse_tests");
    auto actionExpandTree = Ori::Gui::action(tr("&Expand All"), testsTree, SLOT(expandAll()), ":/ori_test_window/expand_tests");
    _actionSaveLog = Ori::Gui::toggledAction(tr("Save Results to Log File"), this, nullptr);

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->setFloatable(false);
    toolbar->setMovable(false);
    Ori::Gui::populate(toolbar, {
        Ori::Gui::textToolButton(_actionRunAll),
        Ori::Gui::textToolButton(_actionRunSelected),
        nullptr, _actionResetState, actionExpandTree, actionCollapseTree });
    addToolBar(Qt::TopToolBarArea, toolbar);

    menuBar()->addMenu(Ori::Gui::menu(tr("&Test"), { _actionRunAll, _actionRunSelected, nullptr,
        _actionResetState, nullptr, actionExpandTree, actionCollapseTree }));
    menuBar()->addMenu(Ori::Gui::menu(tr("&Options"), { _actionSaveLog }));

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

    testLog = new QPlainTextEdit;
    testLog->setReadOnly(true);
    Ori::Gui::setFontMonospace(testLog);

    auto splitter = Ori::Gui::splitterV(testsTree, testLog);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    _sessionThread = new QThread(this);
    _sessionThread->start();
}

TestWindow::~TestWindow()
{
    _sessionThread->quit();

    Ori::Settings s;

    s.beginGroup("TestsExpanded");
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* it = testsTree->topLevelItem(i);
        s.setValue(it->text(0), it->isExpanded());
        saveExpandedStates(it, it->text(0), s);
    }

    s.storeWindowGeometry("testWindow", this);

    _sessionThread->wait(10000);
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
    testsTree->clear();
    testItems.clear();
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

    resetState();
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
        item->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(test)));
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

        if (root)
            root->addChild(item);
        else
            testsTree->addTopLevelItem(item);

        testItems[test] = item;
    }
}

void TestWindow::resetState()
{
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
    {
        auto item = testsTree->topLevelItem(i);
        getTest(item)->reset();
        resetState(item);
    }

    setStatusInfo(CountRun, 0);
    setStatusInfo(CountPass, 0);
    setStatusInfo(CountFail, 0);

    showItemLog(testsTree->currentItem());
}

void TestWindow::resetState(QTreeWidgetItem *root)
{
    setState(root, TestUnknown);
    root->setText(1, QString());
    for (int i = 0; i < root->childCount(); i++)
    {
        QTreeWidgetItem *child = root->child(i);
        if (child->childCount() == 0)
        {
            setState(child, TestUnknown);
            child->setText(1, QString());
        }
        else resetState(child);
    }
}

void TestWindow::setState(QTreeWidgetItem *item, TestState state)
{
    static QMap<TestState, QIcon> testIcons({
        { TestUnknown, QIcon(":/ori_test_states/undef") },
        { TestRunning, QIcon(":/ori_test_states/run") },
        { TestSuccess, QIcon(":/ori_test_states/pass") },
        { TestFail, QIcon(":/ori_test_states/fail") },
    });
    item->setIcon(0, testIcons[state]);
}

void TestWindow::setStatusInfo(StatusInfoKind kind, int value)
{
    switch (kind)
    {
    case CountTotal:
        labelTotal->setText(QString("   Total: %1   ").arg(value));
        break;

    case CountRun:
        labelRun->setText(QString("   Run: %1   ").arg(value));
        break;

    case CountPass:
        labelPass->setText(QString("   Pass: %1   ").arg(value));
        break;

    case CountFail:
        labelFail->setText(QString("   Fail: %1   ").arg(value));
        break;
    }
}

TestBase* TestWindow::getTest(QTreeWidgetItem *item)
{
    return reinterpret_cast<TestBase*>(item->data(0, Qt::UserRole).value<void*>());
}

void TestWindow::runAll()
{
    QList<QTreeWidgetItem*> roots;
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
        roots.append(testsTree->topLevelItem(i));
    runTestSession(roots);
}

void TestWindow::runSelected()
{
    runTestSession(testsTree->selectedItems());
}

void TestWindow::runTestSession(QList<QTreeWidgetItem*> items)
{
    if (items.isEmpty()) return;
    if (_session) return;

    TestLogger::enable(_actionSaveLog->isChecked());

    setStatusInfo(CountRun, 0);
    setStatusInfo(CountPass, 0);
    setStatusInfo(CountFail, 0);

    TestSuite tests;
    for (auto item : items)
    {
        tests << getTest(item);
        resetState(item);
    }

    _session = new TestSession(tests);
    _session->emitSignals = true;
    _session->moveToThread(_sessionThread);
    connect(_session, &TestSession::testRunning, this, &TestWindow::testRunning);
    connect(_session, &TestSession::testFinished, this, &TestWindow::testFinished);
    connect(_session, &TestSession::sessionFinished, this, &TestWindow::sessionFinished);
    connect(this, &TestWindow::sessionStarted, _session, &TestSession::run);

    int testsCount = _session->testsCount();
    if (testsCount > 1)
    {
        progress->setValue(0);
        progress->setMaximum(testsCount);
        progress->setVisible(true);
    }

    _actionRunAll->setEnabled(false);
    _actionRunSelected->setEnabled(false);
    _actionRunSelected->setEnabled(false);

    emit sessionStarted();
}

void TestWindow::testRunning(TestBase* test)
{
    if (!testItems.contains(test)) return;
    auto item = testItems[test];

    setState(item, TestRunning);
}

void TestWindow::testFinished(TestBase* test)
{
    if (!testItems.contains(test)) return;
    auto item = testItems[test];

    auto res = test->result();
    setState(item, res == TestResult::None ? TestUnknown : (
        res == TestResult::Pass ? TestSuccess : TestFail));
    item->setText(1, test->message());

    setStatusInfo(CountRun, _session->testsRun());
    setStatusInfo(CountPass, _session->testsPass());
    setStatusInfo(CountFail, _session->testsFail());

    progress->setValue(_session->testsRun());
}

void TestWindow::sessionFinished()
{
    _session->deleteLater();
    _session = nullptr;

    _actionRunAll->setEnabled(true);
    _actionRunSelected->setEnabled(true);
    _actionRunSelected->setEnabled(true);

    showItemLog(testsTree->currentItem());

    if (progress->isVisible())
        QTimer::singleShot(500, progress, &QProgressBar::hide);
}

void TestWindow::showItemLog(QTreeWidgetItem *item)
{
    if (item) testLog->setPlainText(getTest(item)->log().join('\n'));
}

} // namespace Testing
} // namespace Ori
