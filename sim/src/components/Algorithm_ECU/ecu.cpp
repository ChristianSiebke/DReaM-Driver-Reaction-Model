/********************************************************************************
 * Copyright (c) 2020-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "ecu.h"

Ecu::Ecu()
{
    anyPrevActivity = false;
    index = -1;
}

int Ecu::Perform(bool coll, bool prio1, bool prio2, bool prio3) {

    if (coll)
    {
        index = 0;
    }
    else if (prio1)
    {
        index = 1;
        anyPrevActivity = true;
    }
    else if (prio2)
    {
        index = 2;
        anyPrevActivity = true;
    }
    else if (prio3)
    {
        index = 3;
        anyPrevActivity = true;
    }
    else if (anyPrevActivity)
    {
        index = 100;
    }

    return index;
}
