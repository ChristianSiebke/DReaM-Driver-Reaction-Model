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
//! @file  ExperimentModel.h
//! @ingroup trafficSimulationPlugin
//! @brief The ExperimentModel class implements the functionality of the
//!        ExperimentInterface class.
//-----------------------------------------------------------------------------

#ifndef EXPERIMENTMODEL_H
#define EXPERIMENTMODEL_H

#include "openPASS-TrafficSimulation/ExperimentInterface.h"

#include <QObject>

//-----------------------------------------------------------------------------
//! @brief The ExperimentModel class implements the functionality of the
//!        ExperimentInterface class.
//-----------------------------------------------------------------------------
class ExperimentModel : public ExperimentInterface
{
    Q_OBJECT


public:
    explicit ExperimentModel(QObject *parent = nullptr);
    virtual ~ExperimentModel() override = default;

public:
    virtual ID getID() const override;

public:
    virtual void setNumberInvocations(NumberInvocations const & _numberInvocations) override;
    virtual NumberInvocations getNumberInvocations() const override;


public:
    virtual void setSeed(Seed const & _seed) override;
    virtual Seed getSeed() const override;

public:
    virtual Libraries * getLibraries() const override;
    virtual LoggingGroups * getLoggingGroups() const override;

private:
    ID id;                                                              //!< The ID of the experiment
    NumberInvocations numberInvocations;                                //!< The number of invocations of this experiment
    Seed seed;                                                          //!< The random seed
    Libraries * const libraries;                                        //!< The pointer to the experiment's map of libraries
    LoggingGroups * const loggingGroups;                                //!< The pointer to the experiment's list of logging groups

public:
    static ID const id_default;                                         //!< The default ID
    static NumberInvocations const numberInvocations_default;           //!< The default number of invocations
    static Seed const seed_default;                                     //!< The default random seed

};

#endif // EXPERIMENTMODEL_H
