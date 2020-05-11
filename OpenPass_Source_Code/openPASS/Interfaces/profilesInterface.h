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

#include <list>
#include <memory>
#include <unordered_map>

#include "Common/sensorDefinitions.h"

using StringProbabilities = std::vector<std::pair<std::string, double>>;
using ProfileGroup = std::unordered_map<std::string, openpass::parameter::ParameterSetLevel1>;
using ProfileGroups = std::unordered_map<std::string, ProfileGroup>;

enum class AgentProfileType
{
    Static,
    Dynamic
};

struct AgentProfile
{
    //Dynamic profile
    StringProbabilities driverProfiles {};
    StringProbabilities vehicleProfiles {};

    //Static profile
    std::string systemConfigFile;
    int systemId;
    std::string vehicleModel;

    AgentProfileType type;
};

struct SensorLink
{
    int sensorId {};
    std::string inputId {};
};

struct VehicleComponent
{
    std::string type {};
    StringProbabilities componentProfiles {};
    std::list<SensorLink> sensorLinks {};
};

struct VehicleProfile
{
    std::string vehicleModel {};
    std::list<VehicleComponent> vehicleComponents {};
    openpass::sensors::Parameters sensors {};
};

//-----------------------------------------------------------------------------
//! Interface provides access to the profiles catalog
//-----------------------------------------------------------------------------
class ProfilesInterface
{
public:
    virtual ~ProfilesInterface() = default;

    /*!
    * \brief Returns a pointer to the agentProfiles
    *
    * @return        agentProfiles
    */
    virtual std::unordered_map<std::string, AgentProfile>& GetAgentProfiles() = 0;

    /*!
    * \brief Returns a pointer to the map of vehicle profiles
    *
    * @return        vehicleProfiles
    */
    virtual std::unordered_map<std::string, VehicleProfile>& GetVehicleProfiles() = 0;

    /*!
    * \brief Returns a pointer to the map of profile groups
    *
    * @return        ProfileGroups
    */
    virtual ProfileGroups& GetProfileGroups() = 0;

    /*!
    * \brief Returns the driver profile probabilities of an agentProfile
    *
    * @param[in]    agentProfileName        Name of the agentProfile from which the probabilities are requested
    * @return       probality map for driver profiles
    */
    virtual StringProbabilities& GetDriverProbabilities(std::string agentProfileName) = 0;

    /*!
    * \brief Returns the vehicle profile probabilities of an agentProfile
    *
    * @param[in]    agentProfileName        Name of the agentProfile from which the probabilities are requested
    * @return       probality map for vehicle profiles
    */
    virtual StringProbabilities& GetVehicleProfileProbabilities(std::string agentProfileName) = 0;

    //! \brief Returns the profile with the specified name in the specified profile gropus
    //!
    //! \param type     type of the profile group
    //! \param name     name of the profile
    //! \return     specified profile
    virtual openpass::parameter::ParameterSetLevel1 GetProfile(std::string type, std::string name) = 0;
};
