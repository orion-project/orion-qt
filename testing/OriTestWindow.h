#ifndef ORI_TEST_WINDOW_H
#define ORI_TEST_WINDOW_H

#include <QMainWindow>

#include "OriTestBase.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QProgressBar;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Ori {

class Translator;
class Settings;

namespace Testing {

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = nullptr);
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
    enum TestState
    {
        TestUnknown,
        TestRunning,
        TestSuccess,
        TestFail
    };

    QAction *actionSaveLog;
    QLabel *labelTotal, *labelRun, *labelPass, *labelFail;
    QTreeWidget *testsTree;
    QPlainTextEdit *testLog;
    QProgressBar *progress;
    int testsTotal;
    Ori::Translator* _translator;

    void resetState(QTreeWidgetItem *root);
    void setState(QTreeWidgetItem *item, TestState state);
    void setStatusInfo(StatusInfoKind kind, int value);
    void runTestSession(QList<QTreeWidgetItem *> items);
    void runTest(QTreeWidgetItem *item, TestSession &session);
    TestBase* getTest(QTreeWidgetItem *item);
    int testsCount(QTreeWidgetItem *item);

    void saveExpandedStates(QTreeWidgetItem* root, const QString& rootPath, Ori::Settings& settings);
    void loadExpandedStates(QTreeWidgetItem* root, const QString& rootPath, Ori::Settings& settings);

private slots:
    void runAll();
    void runSelected();
    void resetState();
    void testSelected(QTreeWidgetItem*, QTreeWidgetItem*);
};

} // namespace Testing
} // namespace Ori

#endif // ORI_TEST_WINDOW_H
