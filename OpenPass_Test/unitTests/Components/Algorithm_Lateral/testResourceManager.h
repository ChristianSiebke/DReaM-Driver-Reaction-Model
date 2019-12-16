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

#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "testAlgorithmLateralImplementation.h"

class TestResourceManager {
public:
    TestResourceManager();
    virtual ~TestResourceManager();

    TestAlgorithmLateralImplementation* stubLateralDriver{nullptr};

private:
    FakeStochastics* fakeStochasticsInterface{nullptr};
    FakeParameter* fakeParameters{nullptr};
    FakeAgent* fakeAgent;
    FakeObservation* stubObservation{nullptr};
    const std::map<int, ObservationInterface*>* stubObservations{nullptr};
};
