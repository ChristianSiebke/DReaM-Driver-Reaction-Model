/****************************
* Copyright (c) 2018 Daimler AG.
* All rights reserved.
*****************************/

#include <QStringList>
#include <QDir>
#include "tst_ut_RouteControl.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QStringList testCmd;
    testCmd << " " << "-o" << "UT_RouteControl_log.txt";
    UT_RouteControl uT_routeCtrl;
    QTest::qExec(&uT_routeCtrl, testCmd);
}
