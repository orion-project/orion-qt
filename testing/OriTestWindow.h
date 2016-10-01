#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>

#include "OriTestBase.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QProgressBar;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;
class QTextBrowser;
QT_END_NAMESPACE

namespace Ori {

class Translator;

namespace Test {

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = 0);
    ~TestWindow();

    void setTests(const TestSuite& tests);
    void setTests(QTreeWidgetItem *root, const TestSuite& tests);

private:
    enum StatusInfoKind
    {
        CountTotal,
        CountRun,
        CountPass,
        CountFail
    };

    QAction *actionSaveLog;
    QLabel *labelTotal, *labelRun, *labelPass, *labelFail;
    QTreeWidget *testsTree;
    QTextBrowser *testLog;
    QProgressBar *progress;
    QTimer *hideProgressTimer;
    int testsTotal;
    Ori::Translator* _translator;

    void resetState(QTreeWidgetItem *root);
    void setState(QTreeWidgetItem *item, bool success);
    void resetStatistics();
    void setStatusInfo(StatusInfoKind kind, int value);
    void runTest(QTreeWidgetItem *item, TestSession &session);
    TestBase* getTest(QTreeWidgetItem *item);
    int testsCount(QTreeWidgetItem *item);

private slots:
    void runAll();
    void runSelected();
    void resetState();
    void testSelected(QTreeWidgetItem*, QTreeWidgetItem*);
};

} // namespace Test
} // namespace Ori

#endif // TESTWINDOW_H
