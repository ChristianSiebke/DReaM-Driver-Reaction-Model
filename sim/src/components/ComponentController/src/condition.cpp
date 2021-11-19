/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "condition.h"
#include <stdexcept>

using namespace ComponentControl;

ComponentState FixedComponentStateExpression::Get(const StateMapping &) const
{
    return value;
}

ComponentState VehicleComponentStateExpression::Get(const StateMapping &componentNameToComponentTypeAndStateMap) const
{
    try
    {
        auto componentInformation = componentNameToComponentTypeAndStateMap.at(component);
        return componentInformation.second;
    }
    catch ([[maybe_unused]] const std::out_of_range & e)
    {
        return ComponentState::Undefined;
    }
}

bool ComponentStateEquality::IsFullfilled(const StateMapping &componentNameToComponentTypeAndStateMap) const
{
    return expression1->Get(componentNameToComponentTypeAndStateMap) == expression2->Get(componentNameToComponentTypeAndStateMap);
}
