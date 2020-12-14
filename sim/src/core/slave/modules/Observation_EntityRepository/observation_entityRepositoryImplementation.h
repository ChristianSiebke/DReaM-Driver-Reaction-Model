/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/*!
* \file  ObservationLogImplementation.h
* \brief Adds the RunStatistic information to the simulation output.
* \details  Writes the RunStatistic information into the simulation output.
*           Also manages the stop reasons of the simulation.
*/
//-----------------------------------------------------------------------------

#pragma once

#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>

#include <QFile>
#include <QTextStream>

#include "common/runtimeInformation.h"
#include "include/dataStoreInterface.h"
#include "include/observationInterface.h"

namespace SimulationSlave {
class EventNetworkInterface;
}

class DataStoreReadInterface;

using Metainfo = std::unordered_map<std::string, std::string>;

static constexpr char NON_PERSISTENT_ENTITIES[]{"Entities/NonPersistent"};
static constexpr char PERSISTENT_ENTITIES[]{"Entities/Persistent"};

/**
 * @brief Definition of the headers, which shall be written to the CSV
 * @note  "id" and "source" are fixed (\see CsvRow), the other columns are for describing metainfo (\see CSV_METAINFO)
 */
static const std::vector<std::string> CSV_HEADERS{"id", "source", "version", "name", "secondary id", "type", "subtype"};

/**
 * @brief Definition of metainfo-mapping, where the first entry maps to the first column after the fixed header columns
 * 
 * Example:
 * If an entity defines "MetaInfo/id", the value will be published to the column "secondary id", otherwise the value will be ""
 * 
 * The structure allows to define alternative mappings by extending the mapping e.g. for ids to {"id", "ID", "Identifier", "__id__"}.
 * If an entity now defines any of the given entries (e.g. "MetaInfo/__id__"), it will be mapped to the same column
 * This helps to group arbitary entities to the same column
 */
static const std::vector<std::vector<std::string>> CSV_METAINFO{{"version"}, {"name"}, {"id"}, {"type"}, {"subtype"}};

const QString CSV_DELIMITER{";"};

/**
 * @brief Single entry holding information for printing entity information into CSV
 */
struct CsvRow
{
    CsvRow(uint64_t id, std::string source, Metainfo metainfo) :
        id{id},
        source{std::move(source)},
        metainfo{std::move(metainfo)}
    {
    }

    const uint64_t id;
    const std::string source;
    const Metainfo metainfo;
};

/** \brief This class adds the RunStatistic information to the simulation output.
//-----------------------------------------------------------------------------
*   \details This class inherits the ObservationLogGeneric which creates the basic simulation output
*            and adds the RunStatistic information to the output.
*            This class also manages the stop reasons of the simulation.
*
*   \ingroup ObservationLog
*/
//-----------------------------------------------------------------------------
class ObservationEntityRepositoryImplementation : ObservationInterface
{
public:
    const std::string COMPONENTNAME = "Observation_EntityRepository";

    ObservationEntityRepositoryImplementation(SimulationSlave::EventNetworkInterface *eventNetwork,
                                              StochasticsInterface *stochastics,
                                              WorldInterface *world,
                                              const ParameterInterface *parameters,
                                              const CallbackInterface *callbacks,
                                              DataStoreReadInterface *dataStore);
    ObservationEntityRepositoryImplementation(const ObservationEntityRepositoryImplementation &) = delete;
    ObservationEntityRepositoryImplementation(ObservationEntityRepositoryImplementation &&) = delete;
    ObservationEntityRepositoryImplementation &operator=(const ObservationEntityRepositoryImplementation &) = delete;
    ObservationEntityRepositoryImplementation &operator=(ObservationEntityRepositoryImplementation &&) = delete;
    virtual ~ObservationEntityRepositoryImplementation() override = default;

    virtual void SlavePreHook() override;
    virtual void SlavePreRunHook() override;
    virtual void SlavePostRunHook(const RunResultInterface &runResult) override;
    virtual void SlaveUpdateHook(int, RunResultInterface &) override
    {
    }
    virtual void MasterPreHook() override
    {
    }
    virtual void MasterPostHook(const std::string &) override
    {
    }
    virtual void SlavePostHook() override;

    virtual const std::string SlaveResultFile() override
    {
        return "";
    }

private:
    /**
     * @brief Get the Entities from the datastore as CSV row container
     * 
     * @param entityBaseKey \see NON_PERSISTENT_ENTITIES or \see PERSISTENT_ENTITIES
     * @return All retrieved entities as individual entries
     */
    std::vector<CsvRow> GetEntities(const std::string &entityBaseKey);

    /**
     * @brief Helper method to retrieve the source attribute from an entity
     * 
     * @param source_key   Attribute key within datastore, e.g. 'Entities/Persistent/0/Source'
     * @return The contained value, or "<error>" if not available
     */
    std::string GetEntitySource(const std::string &source_key);

    /**
     * @brief Helper method to retreive the Metainfo provided by an entity
     * 
     * @param metainfo_key Attribute key within datastore, e.g. 'Entities/Persistent/0/Metainfo'
     * @return key/value string pairs containing the parsed metainfo (\see to_string)
     */
    Metainfo GetEntityMetainfo(const std::string &metainfo_key);

    /**
     * @brief Converts any static datastore value into its string representation
     * 
     * @param values value retrieved from the datastore
     * @return converted string
     */
    const std::string to_string(const openpass::datastore::Values &values);

    /**
     * @brief Takes a CSV row container and writes it to a stream
     * 
     * @param entities the entities as CSV rows (\see GetEntities)
     * @param stream   the open stream, where the data should be written to
     */
    void WriteEntities(const std::vector<CsvRow> &entities, QTextStream &stream);

    DataStoreReadInterface *dataStore;
    int runNumber = 0;
    QString directory;
    QString filenamePrefix{"Repository"};
    bool writePersitent{true};
    bool persistentInSeparateFile{false};
    std::vector<CsvRow> persistentEntities{};
};
