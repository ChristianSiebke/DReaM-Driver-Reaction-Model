#include <cfloat>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "dontCare.h"

#include "fakeStochastics.h"
#include "sampler.h"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::DontCare;
using ::testing::Eq;

const openpass::common::RuntimeInformation fakeRti{openpass::common::Version{0,0,0}, {"", "", ""}};

TEST(Sampler_UnitTests, RollForReturnsTrue)
{
    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.5));

    ASSERT_THAT(Sampler::RollFor(0.5, &fakeStochastic), Eq(true));
}

TEST(Sampler_UnitTests, RollForReturnsFalse)
{
    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.6));

    ASSERT_THAT(Sampler::RollFor(0.5, &fakeStochastic), Eq(false));
}

TEST(Sampler_UnitTests, RollForWithChanceZeroReturnsFalse)
{
    NiceMock<FakeStochastics> fakeStochastic;
    ASSERT_THAT(Sampler::RollFor(0.0, &fakeStochastic), Eq(false));
}

TEST(Sampler_UnitTests, SampleIntProbabilityReturnThirdElement)
{
    IntProbabilities fakeProbabilities = {{0, 0.0}, {1, 0.3}, {2, 0.7}};

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.4));

    int result = Sampler::Sample(fakeProbabilities, &fakeStochastic);

    ASSERT_EQ(result, 2);
}

TEST(Sampler_UnitTests, SampleNormalDistributionProbability_AppropriatelyHandlesZeroProbability)
{
    openpass::parameter::NormalDistribution impossibleParameter = DontCare<openpass::parameter::NormalDistribution>();
    openpass::parameter::NormalDistribution expectedParameter{DontCare<double>(),
                                                                         DontCare<double>(),
                                                                         DontCare<double>(),
                                                                         DontCare<double>()};
    StochasticDistributionProbabilities fakeProbabilities{{impossibleParameter, DBL_EPSILON},
                                                          {expectedParameter, 1.0}};
    NiceMock<FakeStochastics> fakeStochastics;
    ON_CALL(fakeStochastics, GetUniformDistributed(_,_)).WillByDefault(Return(0.5));

    const auto sampledNormalDistributionParameter = Sampler::Sample(fakeProbabilities, &fakeStochastics);

    EXPECT_THAT(sampledNormalDistributionParameter, expectedParameter);
}
