/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2019 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "testResourceManager.h"

using ::testing::Return;
using ::testing::An;
using ::testing::_;
using ::testing::ReturnRef;


TestResourceManager::TestResourceManager()
{
    fakeStochasticsInterface = new testing::NiceMock<FakeStochastics>();
    fakeParameters = new testing::NiceMock<FakeParameter>();
    fakeAgent = new FakeAgent;
    fakePublisher = new testing::NiceMock<FakePublisher>;

    CallbackInterface* callbacks = nullptr;
    std::map<std::string, int> intParameters {{"DebugLoggingType", 0}, {"SensorModelType", 0}};
    std::string componentName   = "AlgorithmLateralDriverModules";

    ON_CALL(*fakeParameters, GetParametersInt()).WillByDefault(testing::ReturnRef(intParameters));

    stubLateralDriver = new (std::nothrow) TestAlgorithmLateralImplementation(
                componentName,
                true,
                50,
                0,
                0,
                100,
                fakeStochasticsInterface,
                fakeParameters,
                fakePublisher,
                callbacks,
                fakeAgent);
}

TestResourceManager::~TestResourceManager()
{
    if (fakeStochasticsInterface)
    {
        delete fakeStochasticsInterface;
        fakeStochasticsInterface = nullptr;
    }
    if (fakeParameters)
    {
        delete fakeParameters;
        fakeParameters = nullptr;
    }
    if (fakeAgent)
    {
        delete fakeAgent;
        fakeAgent = nullptr;
    }
    if (fakePublisher)
    {
        delete fakePublisher;
        fakePublisher = nullptr;
    }
    if (stubLateralDriver)
    {
        delete stubLateralDriver;
        stubLateralDriver = nullptr;
    }
}
