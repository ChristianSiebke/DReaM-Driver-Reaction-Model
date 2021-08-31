/*******************************************************************************
* Copyright (c) 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "OWL/DataTypes.h"

//! Holds a set of linked traffic lights (i.e. for one junction) and manages their states
class TrafficLightController
{
public:
    //! \brief Updates the states of all traffic lights of this controller
    //!
    //! \param time     current simulation timestamp
    void UpdateStates(int time);

    //! Each controller consist of a sequence of phases, that define the state of each traffic light
    struct Phase
    {
        int duration;
        std::vector<std::pair<OWL::Interfaces::TrafficLight*, CommonTrafficLight::State>> states;
    };

    TrafficLightController(std::vector<Phase>&& phases, double delay);

private:
    std::vector<Phase> phases;
    std::vector<Phase>::iterator currentPhase;
    int timeRemainingInCurrentPhase;
    int lastTime;
};

//!Holds all TrafficLightControllers and triggers them each timestep
class TrafficLightNetwork
{
public:
    TrafficLightNetwork();

    //! \brief Updates the states of all traffic lights in the network
    //!
    //! \param time     current simulation timestamp
    void UpdateStates(int time);

    //! \brief Adds a new controller to the network
    //!
    //! \param controller   new controller to add
    void AddController(TrafficLightController&& controller);

private:
    std::vector<TrafficLightController> controllers;
};
