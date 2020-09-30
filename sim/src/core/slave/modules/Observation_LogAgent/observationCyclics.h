/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*               2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/


#pragma once

#include <string>
#include <QString>
#include "include/eventNetworkInterface.h"

//!
//! \brief The ObservationCyclics stores the samples which are logged by various modules
//! and written into the cyclics tag of the simulationOutput.xml
//!
class ObservationCyclics
{
public:
    ObservationCyclics()
    {
    }

    ObservationCyclics(const ObservationCyclics&) = delete;
    ObservationCyclics(ObservationCyclics&&) = delete;
    ObservationCyclics& operator=(const ObservationCyclics&) = delete;
    ObservationCyclics& operator=(ObservationCyclics&&) = delete;

    ~ObservationCyclics() = default;

    /*!
    * Inserts a parameter into the samples.
    * If the key does not exist yet, it inserts it into the list of keys
    * and all previous timesteps are filled with an empty string.
    * If the key exists, but some timesteps are missing for this key, then
    * missing timesteps are filled with an empty string.
    *
    * @param[in]     time       Timestep in milliseconds.
    * @param[in]     key        Name of parameter as string.
    * @param[in]     value      Value of parameter as string.
    */
    void Insert(int time, const std::string &key, const std::string &value);

    /*!
     * \brief Returns the string header that is written into the simulationOuput.xml
     */
    std::string GetHeader();

    /*!
     * \brief Returns the string header that is written into the Cyclics_*.csv including agentId
     */
    std::string GetAgentHeader();

    /*!
     * \brief Returns a single line of samples for the given timestep for writing into the simulationOutput
     */
    std::string GetSamplesLine(uint32_t timeStepNumber);

    /*!
     * \brief Returns for each agent a single line of samples for the given timestep for writing into the cyclics_*.csv
     */
    std::vector<std::string> GetAgentSamplesLine(uint32_t timeStepNumber);

    /*!
     * \brief Returns for each agent a single line of samples for the given timestep for writing into the cyclics_*.csv
     */
    static std::vector<int> FilterSamplesLine(QString samplesLine, QString agentId);

    /*!
     * \brief Returns all timesteps for which samples exist
     */
    std::set<int>& GetTimeSteps()
    {
        return timeSteps;
    }

    /*!
     * \brief Clears all samples for a new run
     */
    void Clear();

    static QString GetSamplesLineByAgent(QString samplesLine, std::vector<int> index);

private:
    std::set<int> timeSteps;
    std::map<std::string, std::vector<std::string>> samples;
};


