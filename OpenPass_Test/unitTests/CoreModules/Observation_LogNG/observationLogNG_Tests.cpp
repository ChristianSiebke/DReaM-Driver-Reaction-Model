/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgentDataPublisher.h"
#include "fakeCallback.h"
#include "fakeDataStore.h"
#include "fakeEventNetwork.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"

#include "Common/runtimeInformation.h"

#include "observation_logNGImplementation.h"

using ::testing::NiceMock;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::ReturnRef;

const openpass::common::RuntimeInformation fakeRti{{openpass::common::Version{0,0,0}}, {"", "", ""}};

TEST(ObservationNG, Compiles)
{
    FakeCallback fakeCallback;
    FakeEventNetwork fakeEventNetwork;
    NiceMock<FakeParameter> fakeParameter;
    FakeStochastics fakeStochastics;
    FakeWorld fakeWorld;

    std::map<std::string, const std::vector<std::string>> fakeStringVectors{{"LoggingGroups", {}}};

    ON_CALL(fakeParameter, GetRuntimeInformation()).WillByDefault(ReturnRef(fakeRti));
    ON_CALL(fakeParameter, GetParametersStringVector()).WillByDefault(ReturnRef(fakeStringVectors));

    auto ong = std::make_unique<ObservationLogNGImplementation>(
                   &fakeEventNetwork,
                   &fakeStochastics,
                   &fakeWorld,
                   &fakeParameter,
                   &fakeCallback);

    ASSERT_THAT(ong, NotNull());
}
