#include "tst_ut_switch.h"

using namespace testing;

TEST(UT_Switch, noActivity)
{
    int index = -1;
    ControlData defaultControl = {0.0, 0.0, 0.0, {0.0, 0.0, 0.0, 0.0}};
    ControlData driver = defaultControl;
    ControlData prio1 = defaultControl;
    ControlData prio2 = defaultControl;
    ControlData prio3 = defaultControl;

    Switch switchAssist;
    ControlData controlResult = switchAssist.Perform(index, driver, prio1, prio2, prio3);

    std::vector<double> expectedBrakeSuperpose = {0.0, 0.0, 0.0, 0.0};
    ASSERT_EQ(controlResult.steer, 0.0);
    ASSERT_EQ(controlResult.throttle, 0.0);
    ASSERT_EQ(controlResult.brakePedal, 0.0);
    ASSERT_THAT(controlResult.brakeSuperpose, expectedBrakeSuperpose);
}

TEST(UT_Switch, prio1Active)
{
    int index = 1;
    ControlData defaultControl = {0.0, 0.0, 0.0, {0.0, 0.0, 0.0, 0.0}};
    ControlData driver = defaultControl;
    ControlData prio1 = {1.0, 0.0, 0.0, {0.0, 0.0, 0.0, 0.0}};
    ControlData prio2 = defaultControl;
    ControlData prio3 = defaultControl;

    Switch switchAssist;
    ControlData controlResult = switchAssist.Perform(index, driver, prio1, prio2, prio3);

    std::vector<double> expectedBrakeSuperpose = {0.0, 0.0, 0.0, 0.0};
    ASSERT_EQ(controlResult.steer, 1.0);
    ASSERT_EQ(controlResult.throttle, 0.0);
    ASSERT_EQ(controlResult.brakePedal, 0.0);
    ASSERT_THAT(controlResult.brakeSuperpose, expectedBrakeSuperpose);
}

TEST(UT_Switch, collisionOccurred)
{
    int index = 0;
    ControlData defaultControl = {0.0, 0.0, 0.0, {0.0, 0.0, 0.0, 0.0}};
    ControlData driver = defaultControl;
    ControlData prio1 = defaultControl;
    ControlData prio2 = defaultControl;
    ControlData prio3 = defaultControl;

    Switch switchAssist;
    ControlData controlResult = switchAssist.Perform(index, driver, prio1, prio2, prio3);

    std::vector<double> expectedBrakeSuperpose = {0.0, 0.0, 0.0, 0.0};
    ASSERT_EQ(controlResult.steer, 0.0);
    ASSERT_EQ(controlResult.throttle, 0.0);
    ASSERT_EQ(controlResult.brakePedal, 1.0);
    ASSERT_THAT(controlResult.brakeSuperpose, expectedBrakeSuperpose);
}

