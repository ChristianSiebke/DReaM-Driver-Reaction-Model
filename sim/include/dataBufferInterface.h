/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
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

namespace openpass::databuffer {

using Key = openpass::type::FlatParameterKey;
using Value = openpass::type::FlatParameterValue;
using Parameter = openpass::type::FlatParameter;
using Tokens = std::vector<Key>;

static const std::string WILDCARD = "*";    //!< Wildcard to match any token inside a key string. Length of 1 is mandatory.
static constexpr char SEPARATOR = '/';   //!< Separator for hierarchical key strings. Length of 1 is mandatory.

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
 * \brief Representation of an entry in the acyclics
 */
struct AcyclicRow
{
    AcyclicRow(openpass::type::EntityId id, Key k, Acyclic data) :
        entityId{id},
        key{k},
        data{data}
    {
    }

    bool operator==(const AcyclicRow &other) const
    {
        return entityId == other.entityId &&
               key == other.key &&
               data == other.data;
    }

    openpass::type::EntityId entityId;       //!< Id of the entity (agent or object)
    Key key;                                 //!< Key (topic) associated with the data
    Acyclic data;                            //!< Acyclic data container
};

/*!
 * \brief Representation of an entry in the cyclics
 */
struct CyclicRow
{
    CyclicRow(openpass::type::EntityId id, Key k, Value v) :
        entityId{id},
        key{k},
        tokens{CommonHelper::TokenizeString(key, SEPARATOR)},
        value{v}
    {
    }

    bool operator==(const CyclicRow &other) const
    {
        return entityId == other.entityId &&
               key == other.key &&
               value == other.value;
    }

    openpass::type::EntityId entityId;       //!< Id of the entity (agent or object)
    Key key;                                 //!< Key (topic) associated with the data
    Tokens tokens;                           //!< Tokenized representation of key
    Value value;                             //!< Data value
};

using Keys = std::vector<Key>;               //!< List of keys
using Values = std::vector<Value>;           //!< List of values
using CyclicRows = std::vector<CyclicRow>;   //!< List of data rows

using CyclicRowRefs = std::vector<std::reference_wrapper<const CyclicRow>>;     //!< List of references to rows
using AcyclicRowRefs = std::vector<std::reference_wrapper<const AcyclicRow>>;   //!< List of references to acyclic rows

/*!
 * \brief A set of cyclic data elements representing a DataInterface query result
 *
 * Basic forward iterator properties are provided for convenient result iteration.
 *
 * \code{.cpp}
 *   const auto cyclicResult = dataBuffer->GetCyclic(std::nullopt, "*");
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
 * \brief A set of acyclic data elements representing a DataInterface query result
 *
 * Basic forward iterator properties are provided for convenient result iteration.
 *
 * \code{.cpp}
 *   const auto acyclicResult = dataBuffer->GetAcyclic(std::nullopt, "*");
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
} // namespace openpass::databuffer

using namespace openpass::databuffer;

/*!
 * \brief The DataReadInterface provides read-only access to the data
 *
 * Topics (see Get* methods) for cyclics, acyclics and statics are independent of each other.
 */
class DataBufferReadInterface
{
public:
    DataBufferReadInterface() = default;
    DataBufferReadInterface(const DataBufferReadInterface &) = delete;
    DataBufferReadInterface(DataBufferReadInterface &&) = delete;
    DataBufferReadInterface &operator=(const DataBufferReadInterface &) = delete;
    DataBufferReadInterface &operator=(DataBufferReadInterface &&) = delete;
    virtual ~DataBufferReadInterface() = default;

    static constexpr bool NO_DESCEND = false;

    /*!
     * \brief Retrieves stored cyclic values
     *
     * \param[in]   time       Timestamp of interest
     * \param[in]   entityId   Entity's id
     * \param[in]   key        Unique topic identification
     */
    virtual std::unique_ptr<CyclicResultInterface> GetCyclic(const std::optional<openpass::type::EntityId> entityId, const Key &key) const = 0;

    /*!
     * \brief Retrieves stored acyclic values
     *
     * \param[in]   time       Timestamp of interest
     * \param[in]   entityId   Entity's id
     * \param[in]   key        Unique topic identification
     *
     * \note Current implementation ignores time and entityId
     */
    virtual std::unique_ptr<AcyclicResultInterface> GetAcyclic(const std::optional<openpass::type::EntityId> entityId, const Key &key) const = 0;

    /*!
     * \brief Retrieves stored static values
     *
     * \param[in]   key        Unique topic identification
     */
    virtual Values GetStatic(const Key &key) const = 0;

    /*!
     * \brief Retrieves keys at a specific node in the hierarchy.
     *
     * The key parameter has to be prefixed with "Cyclics/", "Acyclics/" or "Statics/" to
     * get access to the different types of stored elements.
     *
     * The following example will retrieve the list of agent ids participating in the current simulation run:
     * \code{.cpp}
     * const auto agentIds = dataBuffer.GetKeys("Statics/Agents");
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
 * \brief The DataWriteInterface provides write-only access to the data
 *
 * Topics (see Put* methods) for cyclics, acyclics and statics are independent of each other.
 */
class DataBufferWriteInterface
{
public:
    DataBufferWriteInterface() = default;
    DataBufferWriteInterface(const DataBufferWriteInterface &) = delete;
    DataBufferWriteInterface(DataBufferWriteInterface &&) = delete;
    DataBufferWriteInterface &operator=(const DataBufferWriteInterface &) = delete;
    DataBufferWriteInterface &operator=(DataBufferWriteInterface &&) = delete;
    virtual ~DataBufferWriteInterface() = default;

    /*!
     * \brief Writes cyclic information
     *
     * \param[in]   time       Timestamp associated with the provided key/value [ms]
     * \param[in]   entityId   Id of the associated agent or object
     * \param[in]   key        Unique topic identification
     * \param[in]   value      Value to be written
     */
    virtual void PutCyclic(const openpass::type::EntityId entityId, const Key &key, const Value &value) = 0;

    /*!
     * \brief Writes acyclic information
     *
     * \param[in]   time       Timestamp associated with the provided key/value [ms]
     * \param[in]   entityId   Id of the associated agent
     * \param[in]   key        Unique topic identification
     * \param[in]   acyclic    The acyclic element to be written
     */
    virtual void PutAcyclic(const openpass::type::EntityId entityId, const Key &key, const openpass::databuffer::Acyclic &acyclic) = 0;

    /*!
     * \brief Writes static information
     *
     * \param[in]   key       Unique topic identification
     * \param[in]   value     Value to be written
     * \param[in]   persist   Make value persistent (not affected by Clear())
     */
    virtual void PutStatic(const Key &key, const Value &value, bool persist = false) = 0;

    /*!
     * \brief Clears the data contents, except persistent static data
     */
    virtual void ClearRun() = 0;

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
 * \brief The DataInterface provides read/write access to the data
 *
 * This interface combines DataReadInterface and DataWriteInterface and adds some additional
 * methods required for instantiation by the framework.
 */
class DataBufferInterface : public DataBufferReadInterface, public DataBufferWriteInterface
{
public:
    DataBufferInterface() = default;
    DataBufferInterface(const openpass::common::RuntimeInformation *runtimeInformation, const CallbackInterface *callbacks) :
        runtimeInformation(runtimeInformation),
        callbacks(callbacks)
    {
    }

    DataBufferInterface(const DataBufferInterface &) = delete;
    DataBufferInterface(DataBufferInterface &&) = delete;
    DataBufferInterface &operator=(const DataBufferInterface &) = delete;
    DataBufferInterface &operator=(DataBufferInterface &&) = delete;
    virtual ~DataBufferInterface() override = default;

    /*!
     * \brief Instantiates the data buffer
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
     * \return true if data buffer is instantiated, false otherwise
     */
    virtual bool isInstantiated() const
    {
        return false;
    }

    //! \brief Clears the data of the last timestep, but keeps static data
    virtual void ClearTimeStep() = 0;

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
