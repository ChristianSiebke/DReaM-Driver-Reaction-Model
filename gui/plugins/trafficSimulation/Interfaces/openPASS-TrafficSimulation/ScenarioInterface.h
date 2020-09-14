/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ScenarioInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The Scenario Interface provides setter and getter methods for the combination
//!        XML elements related to an Open Scenario File.
//!
//!        These comprise the selection of the scenario file itself as well
//!        as the selection of the ego agent and other scenario agents.
//-----------------------------------------------------------------------------

#ifndef SCENARIOINTERFACE_H
#define SCENARIOINTERFACE_H

#include <QMap>
#include <QObject>
#include <QPair>
#include <QString>

//-----------------------------------------------------------------------------
//! @brief The Scenario Interface provides setter and getter methods for the combination
//!        XML elements related to an Open Scenario File.
//!
//!        These comprise the selection of the scenario file itself as well
//!        as the selection of the ego agent and other scenario agents.
//-----------------------------------------------------------------------------
class ScenarioInterface : public QObject
{
    Q_OBJECT

public:
    using ScenarioFile = QString;                         //!< The name of the open scenario file
    using Name = QString;                                 //!< The name of the scenario
    using CatalogueName = QString;                        //!< The name of the catalogue in which an agent profile is defined
    using EntryName = QString;                            //!< An entry in the catalogue of agent profiles
    using CatalogueRef = QPair<CatalogueName, EntryName>; //!< A catalogue reference, i.e. a reference to an entry (second) in an agent profiles catalogue (first)
    using Entities = QMap<Name, CatalogueRef>;            //!< The mapping of a scenario agent to an agent profile (catalogue reference)
    using Status = bool;                                  //!< Type characterizing the status of the scenario settings

    explicit ScenarioInterface(QObject *parent = nullptr) :
        QObject(parent)
    {
    }

    virtual ~ScenarioInterface() = default;

Q_SIGNALS:
    //-----------------------------------------------------------------------------
    //! Signal, emitted when the name of the scenario has been modified.
    //-----------------------------------------------------------------------------
    void modifiedName();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the open scenario filename has changed
    //-----------------------------------------------------------------------------
    void modifiedScenarioFile();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the entities map of the scenario setup has been set.
    //-----------------------------------------------------------------------------
    void setEntities();

    //-----------------------------------------------------------------------------
    //! Signal, emitted when the scenario's status has changed.
    //-----------------------------------------------------------------------------
    void modifiedStatus();

public:
    //-----------------------------------------------------------------------------
    //! Sets the name of the scenario to the given name.
    //!
    //! @param[in]      newName     the new name to be adopted for the scenario
    //-----------------------------------------------------------------------------
    virtual void setName(Name const &newName) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current name of the scenario.
    //!
    //! @return           the current name of the scenario
    //-----------------------------------------------------------------------------
    virtual Name getName() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method for setting the status of the scenario configuration to the
    //! value passed as an argument.
    //!
    //! @param[in]      _status     the status
    //-----------------------------------------------------------------------------
    virtual void setStatus(Status const &_status) = 0;

    //-----------------------------------------------------------------------------
    //! Method  which returns the current status of the scenario settings.
    //!
    //! @return                 the current status of the scenario configuration
    //-----------------------------------------------------------------------------
    virtual Status getStatus() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Returns the current name of the open scenario file to which this scenario setup
    //! refers.
    //!
    //! @return           the current name of the open scenario file
    //-----------------------------------------------------------------------------
    virtual ScenarioFile getScenarioFile() const = 0;

    //-----------------------------------------------------------------------------
    //! Sets the filename of the open scenario file to the one passed as an argument.
    //!
    //! @param[in]    filename      the new name of the open scenario file
    //-----------------------------------------------------------------------------
    virtual void setScenarioFile(ScenarioFile const &filename) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Returns the map (= container class) in which all scenario agents information
    //! is registered. This comprises a name of the agent profile (key) and a catalogue
    //! reference (value).
    //!
    //! @return           the map of all scenario agents
    //-----------------------------------------------------------------------------
    virtual Entities *getEntities() const = 0;
};

#endif // SCENARIOINTERFACE_H
