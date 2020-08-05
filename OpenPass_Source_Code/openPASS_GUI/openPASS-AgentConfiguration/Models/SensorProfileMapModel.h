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
//! @file  SensorProfileMapModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of SensorProfileMapInterface.
//-----------------------------------------------------------------------------

#ifndef SENSORPROFILEMAPMODEL_H
#define SENSORPROFILEMAPMODEL_H

#include "openPASS-AgentConfiguration/SensorProfileMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of SensorProfileMapInterface.
//-----------------------------------------------------------------------------
class SensorProfileMapModel : public SensorProfileMapInterface
{

    Q_OBJECT

public:
    SensorProfileMapModel(QObject *parent = nullptr);
    ~SensorProfileMapModel()=default;


public:
    virtual SensorProfileMapInterface::Iterator begin() override;
    virtual SensorProfileMapInterface::ConstIterator constBegin() const override;

public:
    virtual SensorProfileMapInterface::Iterator end() override;
    virtual SensorProfileMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(SensorProfileMapInterface::ID const & id) override;

public:
    virtual bool remove(SensorProfileMapInterface::ID const & id) override;

public:
    virtual SensorProfileMapInterface::Item * getItem(SensorProfileMapInterface::ID const & id) const override;

public:
    virtual void setID(SensorProfileMapInterface::Item  * const item,
                         SensorProfileMapInterface::ID const & id) override;

    virtual SensorProfileMapInterface::ID getID(SensorProfileMapInterface::Item  * const item) const override;

    virtual Name generateName(Type type) override;

public:
    virtual bool contains (SensorProfileMapInterface::ID const & id) const override;
    virtual int count() const override;

public:
    virtual void clear() override;

public:
    virtual QList<ID> keys() const override;
    virtual QList<Item*> values() const override;

private:
    Map sensorProfiles;     //!< the map (container) for registering sensor profiles

};

#endif // SENSORPROFILEMAPMODEL_H
