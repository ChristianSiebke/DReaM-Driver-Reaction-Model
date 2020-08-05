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
//! @file  SensorMapModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of SensorMapInterface.
//-----------------------------------------------------------------------------

#ifndef SENSORMAPMODEL_H
#define SENSORMAPMODEL_H

#include "openPASS-AgentConfiguration/SensorMapInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of SensorMapInterface.
//-----------------------------------------------------------------------------
class SensorMapModel : public SensorMapInterface
{

    Q_OBJECT

public:
    SensorMapModel(QObject *parent=nullptr);
    ~SensorMapModel() = default;

public:
    virtual SensorMapInterface::Iterator begin() override;
    virtual SensorMapInterface::ConstIterator constBegin() const override;

public:
    virtual SensorMapInterface::Iterator end() override;
    virtual SensorMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(SensorMapInterface::ID const & id) override;
    virtual bool add(SensorMapInterface::Item * const item) override;

public:
    virtual bool remove(SensorMapInterface::ID const & id) override;
    virtual bool remove(SensorMapInterface::Item * const item) override;

public:
    virtual SensorMapInterface::Item * getItem(SensorMapInterface::ID const & id) const override;

public:
    virtual void setID(SensorMapInterface::Item  * const item,
                         SensorMapInterface::ID const & id) override;

    virtual SensorMapInterface::ID getID(SensorMapInterface::Item  * const item) const override;

    virtual SensorMapInterface::ID generateID() override;

public:
    virtual bool contains (SensorMapInterface::ID const & id) const override;
    virtual int count() const override;

public:
    virtual void clear() override;

public:
    virtual QList<ID> keys() const override;
    virtual QList<Item*> values() const override;

private:
   SensorMapInterface::Map sensors;     //!< the map (container) for registering sensor data

};

#endif // SENSORMAPMODEL_H
