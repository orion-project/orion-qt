#include "testing/OriTestManager.h"

#include <QApplication>
#include <QMessageBox>

// Test suite can be defined in a separate cpp-file (see mytests1.cpp).
// This file must be attached to pro-file to be compiled,
// and suite must be referenced before be added to suite:
USE_SUITE(SampleTestsSuite)

// This is test suite for TestWindow itself, showing its features:
USE_SUITE(TestWindowTests)

// You can define several tests suites and run then all.
namespace AnotherTestSuite {

    TEST_METHOD(top_level_test_method)
    {
        bool flag = false;
        ASSERT_IS_TRUE(flag)
    }

    TEST_SUITE(
        ADD_TEST(top_level_test_method)
    )
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // It is used as settings storage location.
    app.setOrganizationName("orion_examples");

    // You can check if application has been started with 'test' command-line argument.
    // QApplication must be initialized before, to command-line arguments be processed.
    if (!Ori::Testing::isTesting())
        QMessageBox::information(nullptr, QString::fromLatin1("Ori::Test Example"),
            QString::fromLatin1("Application has not been started in test mode.\n\n"
                "But example tests will be started anyway :-)"), QMessageBox::Ok);

    // Run application in testing mode.
    return Ori::Testing::run(app, {
        ADD_SUITE(SampleTestsSuite),
        ADD_SUITE(AnotherTestSuite),
        ADD_SUITE(TestWindowTests),
    });
}


