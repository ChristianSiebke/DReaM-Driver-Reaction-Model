#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "invocationControl.h"

using ::testing::Eq;
using namespace SimulationSlave;

TEST(InvocationControl, GivenTwoInvocations_ProgressesTwice)
{
    InvocationControl ic(2);

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(0));

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1));

    ASSERT_THAT(ic.Progress(), Eq(false));
}

TEST(InvocationControl, GivenTwoInvocationsAndOneRepetitionInSecondCycle_ProgressesThreeTimes)
{
    InvocationControl ic(2);

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(0));

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1));

    ic.Retry();

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1));

    ASSERT_THAT(ic.Progress(), Eq(false));
}

TEST(InvocationControl, GivenMoreRetriesThanAllowed_StopsProgressing)
{
    InvocationControl ic(2);

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(0));

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1));
    ic.Retry();

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1)) << "retry 1";
    ic.Retry();

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1)) << "retry 2";
    ic.Retry();

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1)) << "retry 3";
    ic.Retry();

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1)) << "retry 4";
    ic.Retry();

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(1)) << "retry 5";
    ic.Retry();

    ASSERT_THAT(ic.Progress(), Eq(false));
}

TEST(InvocationControl, GivenAbort_StopsProgressing)
{
    InvocationControl ic(200);

    ASSERT_THAT(ic.Progress(), Eq(true));
    ASSERT_THAT(ic.CurrentInvocation(), Eq(0));

    ic.Abort();
    ASSERT_THAT(ic.Progress(), Eq(false));
}
