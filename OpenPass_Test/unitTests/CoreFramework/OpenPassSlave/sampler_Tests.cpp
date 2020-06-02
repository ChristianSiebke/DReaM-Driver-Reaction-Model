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

const openpass::common::RuntimeInformation fakeRti{openpass::common::Version{0,0,0}, {"", "", ""}};

TEST(Sampler_UnitTests, RollForReturnsTrue)
{
    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.5));

    Sampler sampler(fakeStochastic, fakeRti);

    ASSERT_TRUE(sampler.RollFor(0.5));
}

TEST(Sampler_UnitTests, RollForReturnsFalse)
{
    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.6));

    Sampler sampler(fakeStochastic, fakeRti);

    ASSERT_TRUE(!sampler.RollFor(0.5));
}

TEST(Sampler_UnitTests, RollForWithChanceZeroReturnsFalse)
{
    NiceMock<FakeStochastics> fakeStochastic;
    Sampler sampler(fakeStochastic, fakeRti);
    ASSERT_TRUE(!sampler.RollFor(0.0));
}

TEST(Sampler_UnitTests, RollUniformDistributedVectorIndex)
{
    size_t fakeSize = 5;

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.5));

    Sampler sampler(fakeStochastic, fakeRti);
    unsigned int result = sampler.RollUniformDistributedVectorIndex(fakeSize);

    ASSERT_EQ(result, (unsigned int) 2);
}

TEST(Sampler_UnitTests, RollUniformDistributedVectorIndexWithRollEqualVectorSize)
{
    size_t fakeSize = 4;

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(1.0));

    Sampler sampler(fakeStochastic, fakeRti);
    unsigned int result = sampler.RollUniformDistributedVectorIndex(fakeSize);

    ASSERT_EQ(result, (unsigned int) 3);
}

TEST(Sampler_UnitTests, SampleIntProbabilityReturnThirdElement)
{
    IntProbabilities fakeProbabilities = {{0, 0.0}, {1, 0.3}, {2, 0.7}};

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.4));

    Sampler sampler(fakeStochastic, fakeRti);
    int result = sampler.SampleIntProbability(fakeProbabilities);

    ASSERT_EQ(result, 2);
}

TEST(Sampler_UnitTests, SampleIntProbabilityInvalidProbabilities)
{
    IntProbabilities fakeProbabilities = {{0, 0.0}, {1, 0.3}};

    NiceMock<FakeStochastics> fakeStochastic;
    ON_CALL(fakeStochastic, GetUniformDistributed(_,_)).WillByDefault(Return(0.4));

    Sampler sampler(fakeStochastic, fakeRti);

    try
    {
        sampler.SampleIntProbability(fakeProbabilities);
        ASSERT_TRUE(false);

    }
    catch(...)
    {
        ASSERT_TRUE(true);
    }
}

TEST(Sampler_UnitTests, SampleNormalDistributionProbability_AppropriatelyHandlesZeroProbability)
{
    openpass::parameter::NormalDistribution impossibleParameter = DontCare<openpass::parameter::NormalDistribution>();
    openpass::parameter::NormalDistribution expectedParameter{DontCare<double>(),
                                                                         DontCare<double>(),
                                                                         DontCare<double>(),
                                                                         DontCare<double>()};
    NormalDistributionProbabilities fakeProbabilities{{impossibleParameter, DBL_EPSILON},
                                                      {expectedParameter, 1.0}};
    NiceMock<FakeStochastics> fakeStochastics;
    ON_CALL(fakeStochastics, GetUniformDistributed(_,_)).WillByDefault(Return(0.0));

    Sampler sampler(fakeStochastics, fakeRti);
    const auto sampledNormalDistributionParameter = sampler.SampleNormalDistributionProbability(fakeProbabilities);

    EXPECT_THAT(sampledNormalDistributionParameter, expectedParameter);
}
