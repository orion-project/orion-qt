#ifndef ORI_TEST_WINDOW_H
#define ORI_TEST_WINDOW_H

#include <QMainWindow>

#include "OriTestBase.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QProgressBar;
class QThread;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Ori {

class Settings;

namespace Testing {

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = nullptr);
    ~TestWindow() override;

    void setTests(const TestSuite& tests);

signals:
    void sessionStarted();

private:
    enum StatusInfoKind
    {
        COUNT_TOTAL,
        COUNT_RUN,
        COUNT_PASS,
        COUNT_FAIL
    };
    enum TestState
    {
        TEST_UNKNOWN,
        TEST_RUNNING,
        TEST_SUCCESS,
        TEST_FAIL
    };
    enum TreeColumns {
        COL_NAME,
        COL_DURATION,
        COL_MESSAGE,
        COL_COUNT
    };

    QAction *_actionRunAll, *_actionRunSelected, *_actionResetState, *_actionSaveLog;
    QLabel *labelTotal, *labelRun, *labelPass, *labelFail;
    QTreeWidget *testsTree;
    QPlainTextEdit *testLog;
    QProgressBar *progress;
    QMap<TestBase*, QTreeWidgetItem*> testItems;
    QThread* _sessionThread  = nullptr;
    TestSession* _session = nullptr;
    int testsTotal;

    void setTests(QTreeWidgetItem *root, const TestSuite& tests);
    void resetState(QTreeWidgetItem *root);
    void setState(QTreeWidgetItem *item, TestState state);
    void setStatusInfo(StatusInfoKind kind, int value);
    void runTestSession(QList<QTreeWidgetItem *> items);
    void runTest(QTreeWidgetItem *item, TestSession &session, bool isLastInGroup);
    TestBase* getTest(QTreeWidgetItem *item);

    void saveExpandedStates(QTreeWidgetItem* root, const QString& rootPath, Ori::Settings& settings);
    void loadExpandedStates(QTreeWidgetItem* root, const QString& rootPath, Ori::Settings& settings);

    void testRunning(TestBase* test);
    void testFinished(TestBase* test);
    void sessionFinished();

private slots:
    void runAll();
    void runSelected();
    void resetState();
    void showItemLog(QTreeWidgetItem *item);
};

} // namespace Testing
} // namespace Ori

#endif // ORI_TEST_WINDOW_H
