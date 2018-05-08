#include "testing/OriTestManager.h"
#include "tests/orion_tests.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // It is used as settings storage location.
    app.setOrganizationName("orion_examples");

    return Ori::Testing::run(app, {
        ADD_SUITE(Ori::Tests),
    });
}


