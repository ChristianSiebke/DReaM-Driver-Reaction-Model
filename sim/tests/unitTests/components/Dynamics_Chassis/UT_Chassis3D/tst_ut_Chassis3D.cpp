/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <QString>
#include <QtTest>
#include <qDebug>

#include "ForceWheelZ.h"
#include "VehicleBasics.h"
#include "WheelOscillation.h"

class UT_Chassis3D : public QObject
{
    Q_OBJECT

public:
    UT_Chassis3D() = default;
    ~UT_Chassis3D() = default;

private Q_SLOTS:
    void testCase_data();
    void testCase();

private:
    void TestCaseSequence1(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence2(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence3(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence4(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence5(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence6(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence7(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence8(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
    void TestCaseSequence9(double lenLeft, double lenRight, double lenFront, double lenRear,
                           double heightCOG, double mass, double springCoeff, double damperCoeff);
};

void UT_Chassis3D::testCase_data()
{
    // inputs
    QTest::addColumn<double>("inertiaForceX");
    QTest::addColumn<double>("inertiaForceY");

    // fixed parameters
    QTest::addColumn<double>("lenLeft");
    QTest::addColumn<double>("lenRight");
    QTest::addColumn<double>("lenFront");
    QTest::addColumn<double>("lenRear");
    QTest::addColumn<double>("heightCOG");
    QTest::addColumn<double>("mass");
    QTest::addColumn<double>("springCoeff");
    QTest::addColumn<double>("damperCoeff");

    // outputs
    QTest::addColumn<double>("expectedForceFL");
    QTest::addColumn<double>("expectedForceFR");
    QTest::addColumn<double>("expectedForceRL");
    QTest::addColumn<double>("expectedForceRR");

    double lenLeft = 0.74, lenRight = 0.74;
    double lenFront = 1.15, lenRear = 1.26;
    double heightCOG = 0.59;
    double mass = 1350.0;

    double springCoeff = 1200000;
    double damperCoeff = 12000;

    TestCaseSequence1(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence2(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence3(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence4(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence5(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence6(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence7(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence8(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
    TestCaseSequence9(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass, springCoeff, damperCoeff);
}

void UT_Chassis3D::TestCaseSequence1(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC1";
    double inertiaForceX = 336.0, inertiaForceY = 0.0;  // inputs

    // expected outputs
    double expectedForceFL = 41.131129013001;
    double expectedForceFR = 41.131129013001;
    double expectedForceRL = -41.126350472185;
    double expectedForceRR = -41.126350472185;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence2(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC2";
    double inertiaForceX = -336.0, inertiaForceY = 0.0;  // inputs

    // expected outputs
    double expectedForceFL = -41.126132377798;
    double expectedForceFR = -41.126132377798;
    double expectedForceRL = 41.130910893204;
    double expectedForceRR = 41.130910893204;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence3(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC3";
    double inertiaForceX = 0.0, inertiaForceY = 2.0;  // inputs

    // expected outputs
    double expectedForceFL = 0.416844464837;
    double expectedForceFR = -0.416843995407;
    double expectedForceRL = 0.380453281399;
    double expectedForceRR = -0.380452852951;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence4(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC4";
    double inertiaForceX = 0.0, inertiaForceY = -2.0;  // inputs

    // expected outputs
    double expectedForceFL = -0.416843995407;
    double expectedForceFR = 0.416844464837;
    double expectedForceRL = -0.380452852951;
    double expectedForceRR = 0.380453281399;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence5(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC5";
    double inertiaForceX = 336.0, inertiaForceY = 2.0;  // inputs

    // expected outputs
    double expectedForceFL = 41.547953203216;
    double expectedForceFR = 40.714305292210;
    double expectedForceRL = -40.745876917874;
    double expectedForceRR = -41.506823598041;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence6(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC6";
    double inertiaForceX = -336.0, inertiaForceY = 2.0;  // inputs

    // expected outputs
    double expectedForceFL = -40.709267640182;
    double expectedForceFR = -41.542996645978;
    double expectedForceRL = 41.511343900113;
    double expectedForceRR = 40.750478314735;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence7(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC7";
    double inertiaForceX = 336.0, inertiaForceY = -2.0;  // inputs

    // expected outputs
    double expectedForceFL = 40.714305292210;
    double expectedForceFR = 41.547953203216;
    double expectedForceRL = -41.506823598041;
    double expectedForceRR = -40.745876917874;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence8(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC8";
    double inertiaForceX = -336.0, inertiaForceY = -2.0;  // inputs

    // expected outputs
    double expectedForceFL = -41.542996645978;
    double expectedForceFR = -40.709267640182;
    double expectedForceRL = 40.750478314735;
    double expectedForceRR = 41.511343900113;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::TestCaseSequence9(double lenLeft, double lenRight, double lenFront, double lenRear,
                                     double heightCOG, double mass, double springCoeff, double damperCoeff)
{
    QString testCaseTitle = "TC9";
    double inertiaForceX = 0.0, inertiaForceY = 0.0;  // inputs

    // expected outputs
    double expectedForceFL = 0.0;
    double expectedForceFR = 0.0;
    double expectedForceRL = 0.0;
    double expectedForceRR = 0.0;

    QTest::newRow(testCaseTitle.toLatin1())
                << inertiaForceX << inertiaForceY  // inputs
                << lenLeft << lenRight << lenFront << lenRear << heightCOG << mass << springCoeff << damperCoeff // fixed values
                << expectedForceFL << expectedForceFR  << expectedForceRL << expectedForceRR; // outputs
}

void UT_Chassis3D::testCase()
{
    // inputs
    QFETCH(double, inertiaForceX);
    QFETCH(double, inertiaForceY);

    // fixed values
    QFETCH(double, lenLeft);
    QFETCH(double, lenRight);
    QFETCH(double, lenFront);
    QFETCH(double, lenRear);
    QFETCH(double, heightCOG);
    QFETCH(double, mass);
    QFETCH(double, springCoeff);
    QFETCH(double, damperCoeff);

    // outputs
    QFETCH(double, expectedForceFL);
    QFETCH(double, expectedForceFR);
    QFETCH(double, expectedForceRL);
    QFETCH(double, expectedForceRR);

    VehicleBasics carParam(lenLeft, lenRight, lenFront, lenRear, heightCOG, mass);
    ForceWheelZ wheelForces;

    WheelOscillation oscillations[NUMBER_WHEELS];
    for(int i=0; i < NUMBER_WHEELS; i++)
    {
        oscillations[i].Init(i, 0.01, springCoeff, damperCoeff);
    }

    double resForces[NUMBER_WHEELS];
    double wheelMass;
    double pitchZ = 0.0, rollZ = 0.0;
    for(int i=0; i < 100; i++)
    {
        wheelForces.CalForce(inertiaForceX, inertiaForceY, pitchZ, rollZ, carParam);
        for(int k=0; k < NUMBER_WHEELS; k++)
        {
            wheelMass = carParam.GetWheelMass(k); // the mass is distributed based on deformation
            resForces[k] = wheelForces.GetForce(k); // -wheelMass * GRAVITY_ACC
            oscillations[k].Perform(resForces[k], wheelMass);
        }
        pitchZ = (oscillations[0].GetCurZPos() + oscillations[1].GetCurZPos() - oscillations[2].GetCurZPos() - oscillations[3].GetCurZPos()) / 2.0;
        rollZ = (oscillations[0].GetCurZPos() + oscillations[2].GetCurZPos() - oscillations[1].GetCurZPos() - oscillations[3].GetCurZPos()) / 2.0;
    }

    qDebug("additional Z Forces %.12f, %.12f, %.12f, %.12f", resForces[0], resForces[1], resForces[2], resForces[3]);

    QCOMPARE(resForces[0], expectedForceFL);
    QCOMPARE(resForces[1], expectedForceFR);
    QCOMPARE(resForces[2], expectedForceRL);
    QCOMPARE(resForces[3], expectedForceRR);
}

QTEST_MAIN(UT_Chassis3D);

#include "tst_ut_Chassis3D.moc"
