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

#include "vehicleComponentEvent.h"
#include "Interfaces/signalInterface.h"

/*!
 * ----------------------------------------------------------------------------
 * \brief The ComponentWarningEvent class contains all information related to
 *        an event emitted in response to a Component Warning.
 * ----------------------------------------------------------------------------
 */
class ComponentWarningEvent : public VehicleComponentEvent
{
public:
    ComponentWarningEvent(const int time,
                          const std::string& source,
                          const int agentId,
                          const std::string& componentName,
                          const ComponentWarningInformation& warning):
        VehicleComponentEvent(time,
                              "",
                              source,
                              agentId),
        componentName(componentName),
        warning(warning)
    {}

    ComponentWarningEvent(const ComponentWarningEvent&) = delete;
    ComponentWarningEvent(ComponentWarningEvent&&) = delete;
    ComponentWarningEvent& operator=(const ComponentWarningEvent&) = delete;
    ComponentWarningEvent& operator=(ComponentWarningEvent&&) = delete;
    virtual ~ComponentWarningEvent() override = default;

    /*!
     * \brief Returns the component name for which the event is targeted
     * @return Component Name
     */
    std::string GetComponentName() const
    {
        return componentName;
    }

    /*!
     * \brief GetComponentWarningInformation gets the information regarding the warning from the event
     * \return Component warning information
     */
    ComponentWarningInformation GetComponentWarningInformation() const
    {
        return warning;
    }

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual EventParameters GetParametersAsString() override
    {
        auto eventParameters = VehicleComponentEvent::GetParametersAsString();

        eventParameters.push_back({"ComponentName", componentName});
        eventParameters.push_back({"Activity", warning.activity ? "true" : "false"});
        eventParameters.push_back({"Level", ComponentWarningLevelMapping.at(warning.level)});
        eventParameters.push_back({"Type", ComponentWarningTypeMapping.at(warning.type)});
        eventParameters.push_back({"Intensity", ComponentWarningIntensityMapping.at(warning.intensity)});

        return eventParameters;
    }


private:
    const std::string componentName; //!< The name of the component that has a warning
    const ComponentWarningInformation warning; //!< The collected information regarding the warning
};
