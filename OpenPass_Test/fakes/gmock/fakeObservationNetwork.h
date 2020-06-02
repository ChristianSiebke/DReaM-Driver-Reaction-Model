/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "Interfaces/observationNetworkInterface.h"

class FakeObservationNetwork : public SimulationSlave::ObservationNetworkInterface
{
public:
    MOCK_METHOD4(Instantiate,
        bool(const std::map<int, ObservationInstance>&, StochasticsInterface*, WorldInterface*, EventNetworkInterface*));
    MOCK_METHOD0(GetObservationModules,
        const std::map<int, ObservationModule*>&());
    MOCK_METHOD0(InitAll,
        bool());
    MOCK_METHOD0(InitRun,
        bool());
    MOCK_METHOD2(UpdateTimeStep,
        bool(int, RunResult&));
    MOCK_METHOD1(FinalizeRun,
        bool(const RunResult&));
    MOCK_METHOD0(FinalizeAll,
        bool());
    MOCK_METHOD0(Clear,
        void());
};
