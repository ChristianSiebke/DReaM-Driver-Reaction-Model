/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "observationModule.h"
#include "bindings/observationLibrary.h"
#include "common/log.h"

namespace SimulationSlave {

ObservationModule::ObservationModule(ObservationInterface* implementation,
                                     std::unique_ptr<ParameterInterface> parameter,
                                     ObservationLibrary* library) :
    implementation{implementation},
    parameter{std::move(parameter)},
    library{library}
{}

ObservationModule::~ObservationModule()
{
    library->ReleaseObservationModule(this);
}

} // namespace SimulationSlave
