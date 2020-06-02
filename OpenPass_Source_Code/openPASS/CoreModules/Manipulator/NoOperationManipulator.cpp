/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  CollisionManipulator.cpp */
//-----------------------------------------------------------------------------

#include "NoOperationManipulator.h"

#include <QtGlobal>

NoOperationManipulator::NoOperationManipulator(WorldInterface* world,
        SimulationSlave::EventNetworkInterface* eventNetwork,
        const CallbackInterface* callbacks):
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks)
{
    cycleTime = 100;
}

void NoOperationManipulator::Trigger(int time)
{
    Q_UNUSED(time);
}

EventContainer NoOperationManipulator::GetEvents()
{
    return {};
}
