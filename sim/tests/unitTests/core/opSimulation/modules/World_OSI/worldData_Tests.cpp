/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "WorldData.h"

using ::testing::Eq;

struct SetEnvironmentIllumination_Data
{
    double sunIntensity;
    osi3::EnvironmentalConditions_AmbientIllumination expectedLevel;
};

class SetEnvironmentIlluminationTest : public::testing::TestWithParam<SetEnvironmentIllumination_Data>
{
};

TEST_P(SetEnvironmentIlluminationTest, SetCorrectLevelInGroundtruth)
{
    openScenario::EnvironmentAction environmentAction;
    environmentAction.weather.sun.intensity = GetParam().sunIntensity;
    OWL::WorldData worldData{nullptr};

    worldData.SetEnvironment(environmentAction);

    EXPECT_THAT(worldData.GetOsiGroundTruth().environmental_conditions().ambient_illumination(), Eq(GetParam().expectedLevel));
}

INSTANTIATE_TEST_SUITE_P(IlluminationTests, SetEnvironmentIlluminationTest, ::testing::Values(
SetEnvironmentIllumination_Data{0.005, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL1},
SetEnvironmentIllumination_Data{0.5, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL2},
SetEnvironmentIllumination_Data{2.0, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL3},
SetEnvironmentIllumination_Data{5.0, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL4},
SetEnvironmentIllumination_Data{15.0, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL5},
SetEnvironmentIllumination_Data{50.0, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL6},
SetEnvironmentIllumination_Data{500.0, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL7},
SetEnvironmentIllumination_Data{5000.0, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL8},
SetEnvironmentIllumination_Data{2e5, osi3::EnvironmentalConditions_AmbientIllumination_AMBIENT_ILLUMINATION_LEVEL9}));

struct SetEnvironmentFog_Data
{
    double visualRange;
    osi3::EnvironmentalConditions_Fog expectedLevel;
};

class SetEnvironmentFogTest : public::testing::TestWithParam<SetEnvironmentFog_Data>
{
};

TEST_P(SetEnvironmentFogTest, SetCorrectLevelInGroundtruth)
{
    openScenario::EnvironmentAction environmentAction;
    environmentAction.weather.fog.visualRange = GetParam().visualRange;
    OWL::WorldData worldData{nullptr};

    worldData.SetEnvironment(environmentAction);

    EXPECT_THAT(worldData.GetOsiGroundTruth().environmental_conditions().fog(), Eq(GetParam().expectedLevel));
}

INSTANTIATE_TEST_SUITE_P(Fog, SetEnvironmentFogTest, ::testing::Values(
SetEnvironmentFog_Data{10, osi3::EnvironmentalConditions_Fog_FOG_DENSE},
SetEnvironmentFog_Data{100, osi3::EnvironmentalConditions_Fog_FOG_THICK},
SetEnvironmentFog_Data{500, osi3::EnvironmentalConditions_Fog_FOG_LIGHT},
SetEnvironmentFog_Data{1500, osi3::EnvironmentalConditions_Fog_FOG_MIST},
SetEnvironmentFog_Data{3000, osi3::EnvironmentalConditions_Fog_FOG_POOR_VISIBILITY},
SetEnvironmentFog_Data{6000, osi3::EnvironmentalConditions_Fog_FOG_MODERATE_VISIBILITY},
SetEnvironmentFog_Data{2e4, osi3::EnvironmentalConditions_Fog_FOG_GOOD_VISIBILITY},
SetEnvironmentFog_Data{5e4, osi3::EnvironmentalConditions_Fog_FOG_EXCELLENT_VISIBILITY}));
