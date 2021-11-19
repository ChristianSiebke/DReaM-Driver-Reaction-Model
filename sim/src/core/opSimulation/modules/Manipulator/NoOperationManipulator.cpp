/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  CollisionManipulator.cpp */
//-----------------------------------------------------------------------------

#include "NoOperationManipulator.h"

NoOperationManipulator::NoOperationManipulator(WorldInterface *world,
                                               core::EventNetworkInterface *eventNetwork,
                                               const CallbackInterface *callbacks,
                                               [[maybe_unused]] const openScenario::CustomCommandAction action,
                                               [[maybe_unused]] const std::string &eventName) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks)
{
    cycleTime = 100;
}

void NoOperationManipulator::Trigger([[maybe_unused]] int time)
{
}

EventContainer NoOperationManipulator::GetEvents()
{
    return {};
}
