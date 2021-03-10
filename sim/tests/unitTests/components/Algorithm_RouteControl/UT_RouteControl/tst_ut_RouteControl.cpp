/****************************
* Copyright (c) 2018 Daimler AG.
* All rights reserved.
*****************************/

#include <qDebug>
#include "tst_ut_RouteControl.h"
#include "routeControl.h"

using namespace Common;

std::vector<WaypointData> * CreateWayPointData(double cycleTime, double longVelocity)
{
    int pointCount = 120;
    std::vector<WaypointData> *pWaypoints = new std::vector<WaypointData>(pointCount);
    std::vector<WaypointData> &waypointsRef = *pWaypoints;

    double prevPosX = 20.0;
    for(int i = 0; i < pointCount; ++i)
    {
        waypointsRef[i].time = i*10; // in ms
        waypointsRef[i].longVelocity = longVelocity;
        if ((i > 10) && (i < 15))  // introduce velocity decrease
        {
            waypointsRef[i].longVelocity *= 0.7;
        }

        if ((i > 60) && (i < 65))  // introduce velocity increase
        {
            waypointsRef[i].longVelocity *= 1.3;
        }
        waypointsRef[i].position.x = prevPosX + waypointsRef[i].longVelocity * cycleTime;
        waypointsRef[i].position.y = 0.0;
        prevPosX = waypointsRef[i].position.x;
//        qDebug("%.0f;%.2f;%.2f;%.2f", waypointsRef[i].time, waypointsRef[i].position.x, waypointsRef[i].position.y, waypointsRef[i].longVelocity);
    }

    return pWaypoints;
}

void UT_RouteControl::testCase_data()
{
    // inputs
    QTest::addColumn<double>("cycleMin");
    QTest::addColumn<double>("cycleMax");
    QTest::addColumn<double>("posX");
    QTest::addColumn<double>("posY");
    QTest::addColumn<double>("yawAngle");
    QTest::addColumn<double>("longVelocity");

    // fixed parameters
    QTest::addColumn<double>("mass");
    QTest::addColumn<double>("enginePowerMax");
    QTest::addColumn<double>("brakeTorqueMin");
    QTest::addColumn<double>("drivingAggressivity");

    // outputs
    QTest::addColumn<double>("expectedThrottle");
    QTest::addColumn<double>("expectedBrake");
    QTest::addColumn<double>("expectedSteer");

    double mass = 1000.0;
    double enginePowerMax = 100000.0;
    double brakeTorqueMin = -10000.0;
    double drivingAggressivity = 1.0;

    TestCaseSequence1(mass, enginePowerMax, brakeTorqueMin, drivingAggressivity);
    TestCaseSequence2(mass, enginePowerMax, brakeTorqueMin, drivingAggressivity);
    TestCaseSequence3(mass, enginePowerMax, brakeTorqueMin, drivingAggressivity);
    TestCaseSequence4(mass, enginePowerMax, brakeTorqueMin, drivingAggressivity);
    TestCaseSequence5(mass, enginePowerMax, brakeTorqueMin, drivingAggressivity);
    TestCaseSequence6(mass, enginePowerMax, brakeTorqueMin, drivingAggressivity);
}

void UT_RouteControl::TestCaseSequence1(double mass, double enginePowerMax, double brakeTorqueMin, double drivingAggressivity)
{
    QString testCaseTitle = "TC1";

    // inputs
    double cycleMin = 0, cycleMax = 0;
    double posX = 20.0, posY = 0.0;
    double yawAngle = 0.0;
    double longVelocity = 10.0;

    // expected outputs
    double expectedThrottle = 0.0;
    double expectedBrake = 0.0;
    double expectedSteer = 0.0;

    QTest::newRow(testCaseTitle.toLatin1())
                << cycleMin << cycleMax << posX << posY << yawAngle << longVelocity // inputs
                << mass << enginePowerMax << brakeTorqueMin << drivingAggressivity // fixed values
                << expectedThrottle << expectedBrake  << expectedSteer; // outputs
}

void UT_RouteControl::TestCaseSequence2(double mass, double enginePowerMax, double brakeTorqueMin, double drivingAggressivity)
{
    QString testCaseTitle = "TC2";

    // inputs
    double cycleMin = 0, cycleMax = 0;
    double posX = 20.0, posY = 0.0;
    double yawAngle = 0.0;
    double longVelocity = 9.0;

    // expected outputs
    double expectedThrottle = 1.0;
    double expectedBrake = 0.0;
    double expectedSteer = 0.0;

    QTest::newRow(testCaseTitle.toLatin1())
                << cycleMin << cycleMax << posX << posY << yawAngle << longVelocity // inputs
                << mass << enginePowerMax << brakeTorqueMin << drivingAggressivity // fixed values
                << expectedThrottle << expectedBrake  << expectedSteer; // outputs
}

void UT_RouteControl::TestCaseSequence3(double mass, double enginePowerMax, double brakeTorqueMin, double drivingAggressivity)
{
    QString testCaseTitle = "TC3";

    // inputs
    double cycleMin = 0, cycleMax = 0;
    double posX = 20.0, posY = 0.0;
    double yawAngle = 0.0;
    double longVelocity = 11.0;

    // expected outputs
    double expectedThrottle = 0.0;
    double expectedBrake = 0.824318181818;
    double expectedSteer = 0.0;

    QTest::newRow(testCaseTitle.toLatin1())
                << cycleMin << cycleMax << posX << posY << yawAngle << longVelocity // inputs
                << mass << enginePowerMax << brakeTorqueMin << drivingAggressivity // fixed values
                << expectedThrottle << expectedBrake  << expectedSteer; // outputs
}

void UT_RouteControl::TestCaseSequence4(double mass, double enginePowerMax, double brakeTorqueMin, double drivingAggressivity)
{
    QString testCaseTitle = "TC4";

    // inputs
    double cycleMin = 0, cycleMax = 10;
    double posX = 20.0, posY = 0.0;
    double yawAngle = 0.0;
    double longVelocity = 10.0;

    // expected outputs
    double expectedThrottle = 0.0;
    double expectedBrake = 0.0;
    double expectedSteer = 0.0;

    QTest::newRow(testCaseTitle.toLatin1())
                << cycleMin << cycleMax << posX << posY << yawAngle << longVelocity // inputs
                << mass << enginePowerMax << brakeTorqueMin << drivingAggressivity // fixed values
                << expectedThrottle << expectedBrake  << expectedSteer; // outputs
}

void UT_RouteControl::TestCaseSequence5(double mass, double enginePowerMax, double brakeTorqueMin, double drivingAggressivity)
{
    QString testCaseTitle = "TC5";

    // inputs
    double cycleMin = 9, cycleMax = 19;
    double posX = 21.10, posY = 0.0;
    double yawAngle = 0.0;
    double longVelocity = 10.0;

    // expected outputs
    double expectedThrottle = 0.0;
    double expectedBrake = 0.06075;
    double expectedSteer = 0.0;

    QTest::newRow(testCaseTitle.toLatin1())
                << cycleMin << cycleMax << posX << posY << yawAngle << longVelocity // inputs
                << mass << enginePowerMax << brakeTorqueMin << drivingAggressivity // fixed values
                << expectedThrottle << expectedBrake  << expectedSteer; // outputs
}

void UT_RouteControl::TestCaseSequence6(double mass, double enginePowerMax, double brakeTorqueMin, double drivingAggressivity)
{
    QString testCaseTitle = "TC6";

    // inputs
    double cycleMin = 58, cycleMax = 68;
    double posX = 25.78, posY = 0.0;
    double yawAngle = 0.0;
    double longVelocity = 10.0;

    // expected outputs
    double expectedThrottle = 1.0;
    double expectedBrake = 0.0;
    double expectedSteer = 0.0;

    QTest::newRow(testCaseTitle.toLatin1())
                << cycleMin << cycleMax << posX << posY << yawAngle << longVelocity // inputs
                << mass << enginePowerMax << brakeTorqueMin << drivingAggressivity // fixed values
                << expectedThrottle << expectedBrake  << expectedSteer; // outputs
}

void UT_RouteControl::testCase()
{
    // inputs
    QFETCH(double, cycleMin);
    QFETCH(double, cycleMax);
    QFETCH(double, posX);
    QFETCH(double, posY);
    QFETCH(double, yawAngle);
    QFETCH(double, longVelocity);

    // fixed values
    QFETCH(double, mass);
    QFETCH(double, enginePowerMax);
    QFETCH(double, brakeTorqueMin);
    QFETCH(double, drivingAggressivity);

    // outputs
    QFETCH(double, expectedThrottle);
    QFETCH(double, expectedBrake);
    QFETCH(double, expectedSteer);

    double cycleTime = 0.01;
    RouteControl routeControl(cycleTime);

    routeControl.SetVehicleProperties(mass, enginePowerMax, brakeTorqueMin, drivingAggressivity);

    std::vector<WaypointData> *pWaypoints = CreateWayPointData(cycleTime, 10);
    routeControl.SetRequestedTrajectory(*pWaypoints);

    double curPosX = posX;
    for(int i = cycleMin; i < cycleMax+1; i++)
    {
        int time = i*10;
        routeControl.Perform(time, curPosX, posY, yawAngle, longVelocity);
        qDebug("time %d, pos(%.2f, %.2f), yaw %.2f, velocity %.4f, Throttle %.12f, Brake %.12f, Steer %.12f",
                  time, curPosX, posY, yawAngle, longVelocity,
                  routeControl.GetThrottlePedal(), routeControl.GetBrakePedal(), routeControl.GetFrontWheelAngle());

        curPosX += longVelocity * cycleTime;
    }

    QCOMPARE(routeControl.GetThrottlePedal(), expectedThrottle);
    QCOMPARE(routeControl.GetBrakePedal(), expectedBrake);
    QCOMPARE(routeControl.GetFrontWheelAngle(), expectedSteer);
}
