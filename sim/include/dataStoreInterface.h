/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "common/globalDefinitions.h"
#include "common/openPassTypes.h"
#include "common/runtimeInformation.h"
#include "include/callbackInterface.h"
#include "common/commonTools.h"

namespace openpass::datastore {

using Key = openpass::type::FlatParameterKey;
using Value = openpass::type::FlatParameterValue;
using Parameter = openpass::type::FlatParameter;
using Tokens = std::vector<Key>;

static const std::string WILDCARD = "*";    //!< Wildcard to match any token inside a DataStore key string. Length of 1 is mandatory.
static constexpr char SEPARATOR = '/';   //!< Separator for hierarchical DataStore key strings. Length of 1 is mandatory.

/*!
 * \brief Representation of an component event
 */
class ComponentEvent
{
public:
    ComponentEvent(openpass::type::FlatParameter parameter) :
        parameter{std::move(parameter)}
    {
    }

public:
    openpass::type::FlatParameter parameter;   //!< Generic parameter set associated with this event
};

/*!
 * \brief Represents an acyclic occurence, like an event
 */
class Acyclic
{
public:
    Acyclic() = default;
    Acyclic(std::string name, openpass::type::TriggeringEntities triggeringEntities, openpass::type::AffectedEntities affectedEntities, Parameter parameter) :
        name{std::move(name)},
        triggeringEntities{std::move(triggeringEntities)},
        affectedEntities{std::move(affectedEntities)},
        parameter{std::move(parameter)}
    {
    }

    Acyclic(std::string name, openpass::type::EntityId entity, openpass::type::FlatParameter parameter) :
        name{std::move(name)},
        parameter{std::move(parameter)}
    {
        triggeringEntities.entities.push_back(entity);
    }

    bool operator==(const Acyclic& other) const
    {
        return name == other.name &&
               triggeringEntities.entities == other.triggeringEntities.entities &&
               affectedEntities.entities == other.affectedEntities.entities &&
               parameter == other.parameter;
    }

    std::string name;                         //!< Name (or identifier) of this occurence
    openpass::type::TriggeringEntities triggeringEntities;    //!< List of entities causing this occurence
    openpass::type::AffectedEntities affectedEntities;        //!< List of entities affected by this occurence
    Parameter parameter;                      //!< Generic parameter set associated with this occurence
};

/*!
 * \brief Representation of an entry in the DataStore acyclics
 */
struct AcyclicRow
{
    AcyclicRow(openpass::type::Timestamp ts, openpass::type::EntityId id, Key k, Acyclic data) :
        timestamp{ts},
        entityId{id},
        key{k},
        data{data}
    {
    }

    bool operator==(const AcyclicRow &other) const
    {
        return timestamp == other.timestamp &&
               entityId == other.entityId &&
               key == other.key &&
               data == other.data;
    }

    openpass::type::Timestamp timestamp;     //!< Simulation time timestamp [ms]
    openpass::type::EntityId entityId;       //!< Id of the entity (agent or object)
    Key key;                                 //!< Key (topic) associated with the data
    Acyclic data;                            //!< Acyclic data container
};

/*!
 * \brief Representation of an entry in the DataStore cyclics
 */
struct CyclicRow
{
    CyclicRow(openpass::type::Timestamp ts, openpass::type::EntityId id, Key k, Value v) :
        timestamp{ts},
        entityId{id},
        key{k},
        tokens{CommonHelper::TokenizeString(key, SEPARATOR)},
        value{v}
    {
    }

    bool operator==(const CyclicRow &other) const
    {
        return timestamp == other.timestamp &&
               entityId == other.entityId &&
               key == other.key &&
               value == other.value;
    }

    openpass::type::Timestamp timestamp;     //!< Simulation time timestamp [ms]
    openpass::type::EntityId entityId;       //!< Id of the entity (agent or object)
    Key key;                                 //!< Key (topic) associated with the data
    Tokens tokens;                           //!< Tokenized representation of key
    Value value;                             //!< Data value
};

using Keys = std::vector<Key>;               //!< List of keys used by the DataStore
using Values = std::vector<Value>;           //!< List of values used by the DataStore
using CyclicRows = std::vector<CyclicRow>;   //!< List of data rows used by the DataStore

using CyclicRowRefs = std::vector<std::reference_wrapper<const CyclicRow>>;     //!< List of references to rows inside the DataStore
using AcyclicRowRefs = std::vector<std::reference_wrapper<const AcyclicRow>>;   //!< List of references to acyclic rows inside the DataStore

/*!
 * \brief A set of cyclic data elements representing a DataStore query result
 *
 * Basic forward iterator properties are provided for convenient result iteration.
 *
 * \code{.cpp}
 *   const auto cyclicResult = dataStore->GetCyclic(std::nullopt, std::nullopt, "*");
 *
 *   for (const CyclicRow& row : *cyclicResult)
 *   {
 *      ...
 *   }
 * \endcode
 *
 */
class CyclicResultInterface
{
public:
    virtual ~CyclicResultInterface() = default;

    virtual size_t size() const = 0;
    virtual const CyclicRow& at(const size_t) const = 0;
    virtual CyclicRowRefs::const_iterator begin() const = 0;
    virtual CyclicRowRefs::const_iterator end() const = 0;
};

/*!
 * \brief A set of acyclic data elements representing a DataStore query result
 *
 * Basic forward iterator properties are provided for convenient result iteration.
 *
 * \code{.cpp}
 *   const auto acyclicResult = dataStore->GetAcyclic(std::nullopt, std::nullopt, "*");
 *
 *   for (const AcyclicRow& row : *acyclicResult)
 *   {
 *      ...
 *   }
 * \endcode
 *
 */
class AcyclicResultInterface
{
public:
    virtual ~AcyclicResultInterface() = default;

    virtual size_t size() const = 0;
    virtual const AcyclicRow& at(const size_t) const = 0;
    virtual AcyclicRowRefs::const_iterator begin() const = 0;
    virtual AcyclicRowRefs::const_iterator end() const = 0;
};
} // namespace openpass::datastore

using namespace openpass::datastore;

/*!
 * \brief The DataStoreReadInterface provides read-only access to an underlying DataStore implementation
 *
 * Topics (see Get* methods) for cyclics, acyclics and statics are independent of each other.
 */
class DataStoreReadInterface
{
public:
    DataStoreReadInterface() = default;
    DataStoreReadInterface(const DataStoreReadInterface &) = delete;
    DataStoreReadInterface(DataStoreReadInterface &&) = delete;
    DataStoreReadInterface &operator=(const DataStoreReadInterface &) = delete;
    DataStoreReadInterface &operator=(DataStoreReadInterface &&) = delete;
    virtual ~DataStoreReadInterface() = default;

    static constexpr bool NO_DESCEND = false;

    /*!
     * \brief Retrieves stored cyclic values from the data store
     *
     * \param[in]   time       Timestamp of interest
     * \param[in]   entityId   Entity's id
     * \param[in]   key        Unique topic identification
     */
    virtual std::unique_ptr<CyclicResultInterface> GetCyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const = 0;

    /*!
     * \brief Retrieves stored acyclic values from the data store
     *
     * \param[in]   time       Timestamp of interest
     * \param[in]   entityId   Entity's id
     * \param[in]   key        Unique topic identification
     *
     * \note Current implementation ignores time and entityId
     */
    virtual std::unique_ptr<AcyclicResultInterface> GetAcyclic(const std::optional<openpass::type::Timestamp> time, const std::optional<openpass::type::EntityId> entityId, const Key &key) const = 0;

    /*!
     * \brief Retrieves stored static values from the data store
     *
     * \param[in]   key        Unique topic identification
     */
    virtual Values GetStatic(const Key &key) const = 0;

    /*!
     * \brief Retrieves keys at a specific node in the DataStore hierarchy.
     *
     * The key parameter has to be prefixed with "Cyclics/", "Acyclics/" or "Statics/" to
     * get access to the different types of stored elements.
     *
     * The following example will retrieve the list of agent ids participating in the current simulation run:
     * \code{.cpp}
     * const auto agentIds = dataStore.GetKeys("Statics/Agents");
     * \endcode
     *
     * The following example will retrieve a list of instantiated sensors for agent 3:
     * \code{.cpp}
     * const std::string keyPrefix = "Agents/3/Vehicle/Sensors";
     * \endcode
     *
     * \param[in]   key  Unique topic identification, including prefix
     */
    virtual Keys GetKeys(const Key &key) const = 0;

    /*!
     * \brief Provides callback to LOG() macro
     *
     * \param[in]   logLevel   Importance of log
     * \param[in]   file       Name of file where log is called
     * \param[in]   line       Line within file where log is called
     * \param[in]   message    Message to log
     */
    virtual void Log(CbkLogLevel logLevel, const char *file, int line, const std::string &message) const = 0;
};

/*!
 * \brief The DataStoreWriteInterface provides write-only access to an underlying DataStore implementation
 *
 * Topics (see Put* methods) for cyclics, acyclics and statics are independent of each other.
 */
class DataStoreWriteInterface
{
public:
    DataStoreWriteInterface() = default;
    DataStoreWriteInterface(const DataStoreWriteInterface &) = delete;
    DataStoreWriteInterface(DataStoreWriteInterface &&) = delete;
    DataStoreWriteInterface &operator=(const DataStoreWriteInterface &) = delete;
    DataStoreWriteInterface &operator=(DataStoreWriteInterface &&) = delete;
    virtual ~DataStoreWriteInterface() = default;

    /*!
     * \brief Writes cyclic information into the data store
     *
     * \param[in]   time       Timestamp associated with the provided key/value [ms]
     * \param[in]   entityId   Id of the associated agent or object
     * \param[in]   key        Unique topic identification
     * \param[in]   value      Value to be written
     */
    virtual void PutCyclic(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key &key, const Value &value) = 0;

    /*!
     * \brief Writes acyclic information into the data store
     *
     * \param[in]   time       Timestamp associated with the provided key/value [ms]
     * \param[in]   entityId   Id of the associated agent
     * \param[in]   key        Unique topic identification
     * \param[in]   acyclic    The acyclic element to be written
     */
    virtual void PutAcyclic(const openpass::type::Timestamp time, const openpass::type::EntityId entityId, const Key &key, const openpass::datastore::Acyclic &acyclic) = 0;

    /*!
     * \brief Writes static information into the data store
     *
     * \param[in]   key       Unique topic identification
     * \param[in]   value     Value to be written
     * \param[in]   persist   Make value persistent (not affected by Clear())
     */
    virtual void PutStatic(const Key &key, const Value &value, bool persist = false) = 0;

    /*!
     * \brief Clears the datastore contents, except persistant static data
     */
    virtual void Clear() = 0;

    /*!
     * \brief Provides callback to LOG() macro
     *
     * \param[in]   logLevel   Importance of log
     * \param[in]   file       Name of file where log is called
     * \param[in]   line       Line within file where log is called
     * \param[in]   message    Message to log
     */
    virtual void Log(CbkLogLevel logLevel, const char *file, int line, const std::string &message) const = 0;
};

/*!
 * \brief The DataStoreInterface provides read/write access to an underlying DataStore implementation
 *
 * This interface combines DataStoreReadInterface and DataStoreWriteInterface and adds some additional
 * methods required for instantiation by the framework.
 */
class DataStoreInterface : public DataStoreReadInterface, public DataStoreWriteInterface
{
public:
    DataStoreInterface() = default;
    DataStoreInterface(const openpass::common::RuntimeInformation *runtimeInformation, const CallbackInterface *callbacks) :
        runtimeInformation(runtimeInformation),
        callbacks(callbacks)
    {
    }

    DataStoreInterface(const DataStoreInterface &) = delete;
    DataStoreInterface(DataStoreInterface &&) = delete;
    DataStoreInterface &operator=(const DataStoreInterface &) = delete;
    DataStoreInterface &operator=(DataStoreInterface &&) = delete;
    virtual ~DataStoreInterface() override = default;

    /*!
     * \brief Instantiates the data store
     *
     * \return true if instantiation was successful, false otherwise
     */
    virtual bool Instantiate()
    {
        return false;
    }

    /*!
     * \brief Determines the instantiation status
     *
     * \return true if data store is instantiated, false otherwise
     */
    virtual bool isInstantiated() const
    {
        return false;
    }

    void Log(CbkLogLevel logLevel, const char *file, int line, const std::string &message) const override
    {
        if (callbacks)
        {
            callbacks->Log(logLevel,
                           file,
                           line,
                           message);
        }
    }

protected:
    const openpass::common::RuntimeInformation *runtimeInformation; //!< References the configuration parameters
    const CallbackInterface *callbacks;                             //!< References the callback functions of the framework
};
