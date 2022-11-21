/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ObservationModule.h
//! @brief This file contains the internal representation of an observation
//!        module.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <map>

#include "include/observationInterface.h"
#include "include/parameterInterface.h"
#include "bindings/observationLibrary.h"
#include "common/log.h"

namespace core
{

class ObservationModule
{
public:
    ObservationModule(ObservationInterface *implementation,
                      std::unique_ptr<ParameterInterface> parameter,
                      ObservationLibrary *library);
    ObservationModule(const ObservationModule&) = delete;
    ObservationModule(ObservationModule&&) = delete;
    ObservationModule& operator=(const ObservationModule&) = delete;
    ObservationModule& operator=(ObservationModule&&) = delete;
    virtual ~ObservationModule();

    int GetId() const
    {
        return id;
    }

    ObservationInterface *GetImplementation()
    {
        return implementation;
    }

    ObservationLibrary *GetLibrary()
    {
        return library;
    }

private:
    ObservationInterface *implementation;
    std::unique_ptr<ParameterInterface> parameter;
    ObservationLibrary *library;
    int id = 0;
};

} // namespace core


