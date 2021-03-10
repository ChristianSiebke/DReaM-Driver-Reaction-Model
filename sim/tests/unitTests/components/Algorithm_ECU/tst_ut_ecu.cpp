#include "tst_ut_ecu.h"

using namespace testing;

TEST(UT_Ecu, noActivity)
{
    bool coll = false;
    bool prio1 = false;
    bool prio2 = false;
    bool prio3 = false;

    Ecu ecu;
    int indexResult = ecu.Perform(coll, prio1, prio2, prio3);

    ASSERT_EQ(indexResult, -1);
}

TEST(UT_Ecu, prio1Active)
{
    bool coll = false;
    bool prio1 = true;
    bool prio2 = false;
    bool prio3 = false;

    Ecu ecu;
    int indexResult = ecu.Perform(coll, prio1, prio2, prio3);

    ASSERT_EQ(indexResult, 1);
}

TEST(UT_Ecu, collisionOccurred)
{
    bool coll = true;
    bool prio1 = false;
    bool prio2 = false;
    bool prio3 = false;

    Ecu ecu;
    int indexResult = ecu.Perform(coll, prio1, prio2, prio3);

    ASSERT_EQ(indexResult, 0);
}
