/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ObservationNetwork.h
//! @brief This file implements the container of all observation modules.
//-----------------------------------------------------------------------------

#pragma once

#include <map>

#include "common/opExport.h"
#include "common/log.h"
#include "include/worldInterface.h"
#include "include/observationNetworkInterface.h"

namespace core {

class ObservationBinding;
class ObservationModule;
class Stochastics;
class RunResult;

class SIMULATIONCOREEXPORT ObservationNetwork : public ObservationNetworkInterface
{
public:
    ObservationNetwork(std::map<std::string, ObservationBinding>* bindings) :
        bindings(bindings)
    {}
    ObservationNetwork(const ObservationNetwork&) = delete;
    ObservationNetwork(ObservationNetwork&&) = delete;
    ObservationNetwork& operator=(const ObservationNetwork&) = delete;
    ObservationNetwork& operator=(ObservationNetwork&&) = delete;

    virtual ~ObservationNetwork() override;

    virtual bool Instantiate(const ObservationInstanceCollection& observationInstances,
                             StochasticsInterface* stochastics,
                             WorldInterface* world,
                             EventNetworkInterface* eventNetwork,
                             const std::string& sceneryPath,
                             DataBufferReadInterface* dataBuffer) override;
    virtual const std::map<int, ObservationModule*>& GetObservationModules() override;

    virtual bool InitAll() override;
    virtual bool InitRun() override;
    virtual bool UpdateTimeStep(int time, RunResult& runResult) override;
    virtual bool FinalizeRun(const RunResult& result) override;
    virtual bool FinalizeAll() override;
    virtual void Clear() override;

private:
    std::map<std::string, ObservationBinding>* bindings;
    std::map<int, ObservationModule*> modules {};
};

} // namespace core


