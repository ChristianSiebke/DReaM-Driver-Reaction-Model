/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include <string>
#include <vector>
#include "common/parameter.h"
#include "include/dataBufferInterface.h"

/**
 * @brief Wrapper for building an ObservationInstance
 */
struct ObservationInstance
{
    int id;     //! for internal use
    std::string libraryName; //! Name of the library to load
    openpass::parameter::ParameterSetLevel1 parameters; //! Parameters of ObservationLibrary
};

using ObservationInstanceCollection = std::vector<ObservationInstance>;
