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
//! @file  ExperimentInterface.h
//! @ingroup trafficSimulationPlugin
//! @brief The Experiment Interface provides setter and getter methods for the XML
//!        elements listed under <ExperimentConfig> in the slaveConfig XML file.
//!
//!        These comprise the number of invocations, the random
//!        seed, the libraries required by the simulation slave and the logging groups.
//!        See the documentation of the simulation framework for the
//!        precise definition of these settings.
//-----------------------------------------------------------------------------

#ifndef EXPERIMENTINTERFACE_H
#define EXPERIMENTINTERFACE_H

#include <QMap>
#include <QObject>
#include <QStringList>

//-----------------------------------------------------------------------------
//! @brief The Experiment Interface provides setter and getter methods for the XML
//!        elements listed under <ExperimentConfig> in the slaveConfig XML file.
//!
//!        These comprise the number of invocations, the random
//!        seed, the libraries required by the simulation slave and the logging groups.
//!        See the documentation of the simulation framework for the
//!        precise definition of these settings.
//-----------------------------------------------------------------------------
class ExperimentInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int;                  //!< The experiment's ID (not used so far)
    using NumberInvocations = int;            //!< The number of invocations
    using Seed = int;                         //!< The random seed used for random number generation
    using Libraries = QMap<QString, QString>; //!< The map of libraries needed by the simulation slave
    using LoggingGroups = QStringList;        //!< The list of logging groups

public:
    explicit ExperimentInterface(QObject *parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ExperimentInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Returns the ID of the experiment.
    //!
    //! @return                 The ID of the experiment
    //! @note                   The ID is not yet used since only one experiment is
    //!                         configured in one slaveConfig XML file.
    //-----------------------------------------------------------------------------
    virtual ID getID() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the number of invocations of the experiment.
    //!
    //! @param[in]      _numberInvocations          the number of invocations to be adopted.
    //-----------------------------------------------------------------------------
    virtual void setNumberInvocations(NumberInvocations const &_numberInvocations) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current number of invocations.
    //!
    //! @return                        the current number of invocations
    //-----------------------------------------------------------------------------
    virtual NumberInvocations getNumberInvocations() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Sets the seed used for random number generation.
    //!
    //! @param[in]      _seed          the random number seed to be adopted
    //-----------------------------------------------------------------------------
    virtual void setSeed(Seed const &_seed) = 0;

    //-----------------------------------------------------------------------------
    //! Returns the current random number seed.
    //!
    //! @return                        the current random number seed
    //-----------------------------------------------------------------------------
    virtual Seed getSeed() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Returns the pointer to the Libraries map.
    //!
    //! @return                the pointer to the libraries map
    //-----------------------------------------------------------------------------
    virtual Libraries *getLibraries() const = 0;

    //-----------------------------------------------------------------------------
    //! Returns the pointer to the list of Logging Groups.
    //!
    //! @return                the pointer to the list of logging groups
    //-----------------------------------------------------------------------------
    virtual LoggingGroups *getLoggingGroups() const = 0;
};

#endif // EXPERIMENTINTERFACE_H
