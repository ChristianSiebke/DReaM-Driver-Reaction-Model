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
#include "Common/globalDefinitions.h"
#include "Interfaces/worldInterface.h"

class NavigationInterface
{
public:
    virtual ~NavigationInterface() = default;
    virtual Route GetRoute(GlobalRoadPosition start) const = 0;
};
