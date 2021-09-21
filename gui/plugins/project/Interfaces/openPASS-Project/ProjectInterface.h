/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ProjectInterface.h
//! @brief The Project Interface (PI) contains the functions setters and getters
//! of the items constituing the project configuration or framework configuration
//! for the simulation.
//! This interface belongs to the Project Plugin (PP).
//!
//-----------------------------------------------------------------------------

#ifndef PROJECTINTERFACE_H
#define PROJECTINTERFACE_H

#include <QDir>
#include <QObject>
#include <QString>

#include "openPASS/ServiceInterface.h"

static ServiceManagerInterface::ID const ProjectInterfaceID =
    ServiceManagerInterfaceID("openPASS.Project.ProjectInterface");

/**
 * @brief This class defines the items constituing the project configuration:
 *      - the components library (path)
 *      - the project results (path)
 *      - the agent configuration file
 *      - the run configuration file
 *      - the scenery configuration file
 *      - the log file for openPASS Simulation Manager
 *      - the log file for openPASS Simulation
 *      - the level of information in log files [0...5]
 *      - the executable openPASS Simulation (path)
 * These items are transmitted as the framework configuration of the simulation
 * for further process to opSimulationManager.
 */
class ProjectInterface : public QObject,
                         public ServiceInterface<ProjectInterface, ProjectInterfaceID>
{
    Q_OBJECT

public:
    using Config = QString;
    using Library = QString;
    using Result = QString;
    using LogOpSimulationManager = QString;
    using LogSimulation = QString;
    using LogLevel = unsigned int;
    using Simulation = QString;

public:
    explicit ProjectInterface(QObject *const parent = nullptr) :
        QObject(parent), ServiceInterface(this)
    {
    }
    virtual ~ProjectInterface() = default;

Q_SIGNALS: // update actions from the Menu-bar
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that the project has been reset successfully.
    //-----------------------------------------------------------------------------
    void cleared() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a project has been loaded successfully.
    //-----------------------------------------------------------------------------
    void loaded() const;

    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that a project has been saved successfully.
    //-----------------------------------------------------------------------------
    void saved() const;

Q_SIGNALS: // update actions from the Browse Buttons
    //-----------------------------------------------------------------------------
    //! Triggers signal to inform that an item from the project configuration has
    //! been modified.
    //-----------------------------------------------------------------------------
    void update();

public:
    //-----------------------------------------------------------------------------
    //! Resets the project configuration.
    //!
    //! @return                         True if project configuration successfully reset.
    //-----------------------------------------------------------------------------
    virtual bool clear() = 0;

    //-----------------------------------------------------------------------------
    //! Loads a project from a file located in the given path \a filepath.
    //!
    //! @param[in]      filepath        The path locating the project configuration file.
    //! @return                         True if project successfully loaded.
    //-----------------------------------------------------------------------------
    virtual bool load(QString const &filepath) = 0;

    //-----------------------------------------------------------------------------
    //! Saves a project to a file stored in the given path \a filepath.
    //!
    //! @param[in]      filepath        The path for storing the project configuration file.
    //! @return                         True if project successfully saved.
    //-----------------------------------------------------------------------------
    virtual bool save(QString const &filepath) const = 0;

public:
    virtual bool setConfigPath(ProjectInterface::Config const &configs) = 0;
    virtual ProjectInterface::Config getConfigPath() const = 0;
    virtual QString relativeToConfigPath(QString const &filepath) const = 0;
    virtual QString absoluteToConfigPath(QString const &filepath) const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the path of the directory containing the components library \a library.
    //!
    //! @param[in]      library         The path of the components library.
    //! @return                         True if the path exists.
    //-----------------------------------------------------------------------------
    virtual bool setLibraryPath(ProjectInterface::Library const &library) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the path of the directory containing the components library.
    //!
    //! @return                         The path of the components library.
    //-----------------------------------------------------------------------------
    virtual ProjectInterface::Library getLibraryPath() const = 0;

    //-----------------------------------------------------------------------------
    //! Gets the directory containing the components library.
    //!
    //! @return                         The components library.
    //-----------------------------------------------------------------------------
    virtual QDir getLibrary() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the path of the directory containing the project results \a result.
    //!
    //! @param[in]      result          The path of the project results.
    //! @return                         True if the path exists.
    //-----------------------------------------------------------------------------
    virtual bool setResultPath(ProjectInterface::Result const &result) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the path of the directory containing the project results.
    //!
    //! @return                         The path of the project results.
    //-----------------------------------------------------------------------------
    virtual ProjectInterface::Result getResultPath() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the path of the directory containing the log file of the opSimulationManager
    //! (Log opSimulationManager) \a logOpSimulationManager.
    //!
    //! @param[in]      logOpSimulationManager       The path of the file Log opSimulationManager.
    //! @return                         True if the path exists.
    //-----------------------------------------------------------------------------
    virtual bool setLogOpSimulationManager(ProjectInterface::LogOpSimulationManager const &logOpSimulationManager) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the path of the directory containing the log file of the opSimulationManager.
    //!
    //! @return                         The path of the Log opSimulationManager.
    //-----------------------------------------------------------------------------
    virtual ProjectInterface::LogOpSimulationManager getLogOpSimulationManager() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the path of the directory containing the log file of the openPASS Simulation
    //! (Log Simulation) \a logSimulation.
    //!
    //! @param[in]      logSimulation        The path of the file Log Simulation.
    //! @return                         True if the path exists.
    //-----------------------------------------------------------------------------
    virtual bool setLogSimulation(ProjectInterface::LogSimulation const &logSimulation) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the path of the directory containing the log file of the openPASS Simulation.
    //!
    //! @return                     The path of the Log Simulation.
    //-----------------------------------------------------------------------------
    virtual ProjectInterface::LogSimulation getLogSimulation() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the level of information \a logLevel in the log files for the current
    //! project.
    //!
    //! @param[in]      logLevel        The level of information in the log files.
    //! @return                         True if log level successfully modified.
    //-----------------------------------------------------------------------------
    virtual bool setLogLevel(ProjectInterface::LogLevel const &logLevel) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the level of information in the log files for the current project.
    //!
    //! @return                         The level of information in the log files.
    //-----------------------------------------------------------------------------
    virtual ProjectInterface::LogLevel getLogLevel() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the path of the executable openPASS simulation.
    //!
    //! @return                     True if the path exists.
    //-----------------------------------------------------------------------------
    virtual bool setSimulationExe(ProjectInterface::Simulation const &simulation) = 0;

    //-----------------------------------------------------------------------------
    //! Gets the path of the executable openPASS simulation.
    //!
    //! @return                     The path of the executable.
    //-----------------------------------------------------------------------------
    virtual ProjectInterface::Simulation getSimulationExe() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Verifies if the required entries for the simulation are given.
    //!
    //! @return                     True if all fields filled.
    //-----------------------------------------------------------------------------
    virtual bool getProjectStatus() const = 0;
};

#endif // PROJECTINTERFACE_H
