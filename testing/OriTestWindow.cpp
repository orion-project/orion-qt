#include "OriTestWindow.h"

#include "../tools/OriSettings.h"
#include "../helpers/OriWidgets.h"
#include "../widgets/OriFlatToolBar.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
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

    testsTree = new QTreeWidget;
    testsTree->setColumnCount(COL_COUNT);
    testsTree->setAlternatingRowColors(true);
    testsTree->setHeaderLabels({tr("Test"), tr("Duration"), tr("Message")});
    testsTree->header()->setSectionResizeMode(COL_NAME, QHeaderView::ResizeToContents);
    testsTree->header()->setSectionResizeMode(COL_DURATION, QHeaderView::ResizeToContents);
    testsTree->header()->setSectionResizeMode(COL_MESSAGE, QHeaderView::Stretch);
    connect(testsTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(showItemLog(QTreeWidgetItem*)));

    _actionRunAll = Ori::Gui::action(tr("Run &All"), this, SLOT(runAll()), ":/ori_test_window/run_tests");
    _actionRunSelected = Ori::Gui::action(tr("Run &Selected"), this, SLOT(runSelected()), ":/ori_test_window/run_test");
    _actionStop = Ori::Gui::action(tr("Stop &Execution"), this, SLOT(stopExecution()), ":/ori_test_window/stop");
    _actionStop->setEnabled(false);
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
        Ori::Gui::textToolButton(_actionStop),
        nullptr, _actionResetState, nullptr, actionExpandTree, actionCollapseTree });
    addToolBar(Qt::TopToolBarArea, toolbar);

    menuBar()->addMenu(Ori::Gui::menu(tr("&Test"), { _actionRunAll, _actionRunSelected, _actionStop,
        nullptr, _actionResetState, nullptr, actionExpandTree, actionCollapseTree }));
    menuBar()->addMenu(Ori::Gui::menu(tr("&Options"), { _actionSaveLog }));

    progress = new QProgressBar;
    progress->setMaximumHeight(10);
    progress->setVisible(false);
    testsTotal = 0;
    labelTotal = new QLabel;
    labelRun = new QLabel;
    labelPass = new QLabel;
    labelFail = new QLabel;
    labelDuration = new QLabel;
    setStatusInfo(COUNT_TOTAL, 0);
    setStatusInfo(COUNT_RUN, 0);
    setStatusInfo(COUNT_PASS, 0);
    setStatusInfo(COUNT_FAIL, 0);
    setStatusDuration(0, 0);
    statusBar()->addWidget(labelTotal);
    statusBar()->addWidget(labelRun);
    statusBar()->addWidget(labelPass);
    statusBar()->addWidget(labelFail);
    statusBar()->addWidget(progress);
    statusBar()->addWidget(labelDuration);
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
        s.setValue(it->text(COL_NAME), it->isExpanded());
        saveExpandedStates(it, it->text(COL_NAME), s);
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
            auto path = QString("%1/%2").arg(rootPath, item->text(COL_NAME));
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
    setStatusInfo(COUNT_TOTAL, testsTotal);

    Ori::Settings s;
    s.beginGroup("TestsExpanded");
    for (int i = 0; i < testsTree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* it = testsTree->topLevelItem(i);
        it->setExpanded(s.value(it->text(COL_NAME), true).toBool());
        loadExpandedStates(it, it->text(COL_NAME), s);
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
            auto path = QString("%1/%2").arg(rootPath, item->text(COL_NAME));
            item->setExpanded(settings.value(path, true).toBool());
            loadExpandedStates(item, path, settings);
        }
    }
}

void TestWindow::setTests(QTreeWidgetItem *root, const TestSuite &tests)
{
    for (TestBase *test: tests)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setData(COL_NAME, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(test)));
        item->setText(COL_NAME, test->name());

        TestGroup *group = dynamic_cast<TestGroup*>(test);
        if (group)
        {
            setTests(item, group->tests());

            QFont font = item->font(COL_NAME);
            font.setBold(true);
            item->setFont(COL_NAME, font);
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

    setStatusInfo(COUNT_RUN, 0);
    setStatusInfo(COUNT_PASS, 0);
    setStatusInfo(COUNT_FAIL, 0);
    setStatusDuration(0, 0);

    showItemLog(testsTree->currentItem());
}

void TestWindow::resetState(QTreeWidgetItem *root)
{
    setState(root, TEST_UNKNOWN);
    root->setText(1, QString());
    for (int i = 0; i < root->childCount(); i++)
    {
        QTreeWidgetItem *child = root->child(i);
        if (child->childCount() == 0)
        {
            setState(child, TEST_UNKNOWN);
            child->setText(COL_DURATION, QString());
            child->setText(COL_MESSAGE, QString());
        }
        else resetState(child);
    }
}

void TestWindow::setState(QTreeWidgetItem *item, TestState state)
{
    static QMap<TestState, QIcon> testIcons({
        { TEST_UNKNOWN, QIcon(":/ori_test_states/undef") },
        { TEST_RUNNING, QIcon(":/ori_test_states/run") },
        { TEST_SUCCESS, QIcon(":/ori_test_states/pass") },
        { TEST_FAIL, QIcon(":/ori_test_states/fail") },
    });
    item->setIcon(COL_NAME, testIcons[state]);
}

void TestWindow::setStatusInfo(StatusInfoKind kind, int value)
{
    switch (kind)
    {
    case COUNT_TOTAL:
        labelTotal->setText(QStringLiteral("   Total: %1   ").arg(value));
        break;

    case COUNT_RUN:
        labelRun->setText(QStringLiteral("   Run: %1   ").arg(value));
        break;

    case COUNT_PASS:
        labelPass->setText(QStringLiteral("   Pass: %1   ").arg(value));
        break;

    case COUNT_FAIL:
        labelFail->setText(QStringLiteral("   Fail: %1   ").arg(value));
        break;
    }
}

void TestWindow::setStatusDuration(int64_t tests, int64_t session)
{
    if (tests == 0)
    {
        labelDuration->setText(QStringLiteral("   Duration: NA   "));
        labelDuration->setToolTip(QString());
        return;
    }
    labelDuration->setText(QStringLiteral("   Duration: %1 (%2)   ")
                           .arg(formatDuration(tests))
                           .arg((session == 0) ? QStringLiteral("?") : formatDuration(session)));
    labelDuration->setToolTip(tr("Summary tests duration (Total session duration including all additional times)"));
}

TestBase* TestWindow::getTest(QTreeWidgetItem *item)
{
    return reinterpret_cast<TestBase*>(item->data(COL_NAME, Qt::UserRole).value<void*>());
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

    setStatusInfo(COUNT_RUN, 0);
    setStatusInfo(COUNT_PASS, 0);
    setStatusInfo(COUNT_FAIL, 0);
    setStatusDuration(0, 0);

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
    _actionStop->setEnabled(true);

    emit sessionStarted();
}

void TestWindow::testRunning(TestBase* test)
{
    if (!testItems.contains(test)) return;
    auto item = testItems[test];

    setState(item, TEST_RUNNING);
}

void TestWindow::testFinished(TestBase* test)
{
    if (!testItems.contains(test)) return;
    auto item = testItems[test];

    auto res = test->result();
    setState(item, res == TestResult::None ? TEST_UNKNOWN : (
        res == TestResult::Pass ? TEST_SUCCESS : TEST_FAIL));
    item->setText(COL_MESSAGE, test->message());
    item->setText(COL_DURATION, Ori::Testing::formatDuration(test->duration()));

    setStatusInfo(COUNT_RUN, _session->testsRun());
    setStatusInfo(COUNT_PASS, _session->testsPass());
    setStatusInfo(COUNT_FAIL, _session->testsFail());
    setStatusDuration(_session->testsDuration(), _session->sessionDuration());

    progress->setValue(_session->testsRun());
}

void TestWindow::sessionFinished()
{
    setStatusDuration(_session->testsDuration(), _session->sessionDuration());

    _session->deleteLater();
    _session = nullptr;

    _actionRunAll->setEnabled(true);
    _actionRunSelected->setEnabled(true);
    _actionRunSelected->setEnabled(true);
    _actionStop->setEnabled(false);

    showItemLog(testsTree->currentItem());

    if (_stopTimer && _stopTimer->isActive())
        _stopTimer->stop();
    if (progress->isVisible())
        QTimer::singleShot(500, progress, &QProgressBar::hide);
}

void TestWindow::showItemLog(QTreeWidgetItem *item)
{
    if (item) testLog->setPlainText(getTest(item)->log().join('\n'));
}

void TestWindow::stopExecution()
{
    if (!_session) return;
    _actionStop->setEnabled(false);
    _session->stop();
    _stopTries = 0;
    if (!_stopTimer)
    {
        _stopTimer = new QTimer(this);
        _stopTimer->setInterval(500);
        connect(_stopTimer, SIGNAL(timeout()), this, SLOT(stopTimerTick()));
    }
    _stopTimer->start();
}

void TestWindow::stopTimerTick()
{
    if (!_session)
    {
        _stopTimer->stop();
        return;
    }
    _stopTries++;
    if (_stopTries < 10) return;
    _stopTimer->stop();
    _sessionThread->terminate();
    while (_sessionThread->isRunning())
        _sessionThread->wait(500);
    // `sessionFinished()` was not called and the "Start" action remains disabled
    // it doesn't worth to restart the thread because memory might be corrupted
    QMessageBox::critical(this, windowTitle(), tr("Unable to stop properly.\n"
        "Execution was forced to interrupt.\nThe application must be restarted."));
}

} // namespace Testing
} // namespace Ori
