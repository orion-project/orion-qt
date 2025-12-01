#include "testing/OriTestManager.h"
#include "tests/orion_tests.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    a.setOrganizationName("orion-project.org");
    a.setStyle("fusion");

    return Ori::Testing::run(app, {
        ADD_SUITE(Ori::Tests),
    });
}


