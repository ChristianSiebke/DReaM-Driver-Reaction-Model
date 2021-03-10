/****************************
* Copyright (c) 2017 Daimler AG.
* All rights reserved.
*****************************/

#ifndef TST_UT_ROUTECONTROL_H
#define TST_UT_ROUTECONTROL_H

#include <QString>
#include <QtTest>

class UT_RouteControl : public QObject
{
    Q_OBJECT

public:
    UT_RouteControl() = default;
    ~UT_RouteControl() = default;

private Q_SLOTS:
    void testCase_data();
    void testCase();

private:
    void TestCaseSequence1(double lenLeft, double lenRight, double lenFront, double drivingAggressivity);
    void TestCaseSequence2(double lenLeft, double lenRight, double lenFront, double drivingAggressivity);
    void TestCaseSequence3(double lenLeft, double lenRight, double lenFront, double drivingAggressivity);
    void TestCaseSequence4(double lenLeft, double lenRight, double lenFront, double drivingAggressivity);
    void TestCaseSequence5(double lenLeft, double lenRight, double lenFront, double drivingAggressivity);
    void TestCaseSequence6(double lenLeft, double lenRight, double lenFront, double drivingAggressivity);
};

#endif // TST_UT_ROUTECONTROL_H
