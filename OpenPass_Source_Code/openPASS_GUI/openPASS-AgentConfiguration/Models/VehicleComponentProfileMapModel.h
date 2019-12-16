/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef VEHICLECOMPONENTPROFILEMAPMODEL_H
#define VEHICLECOMPONENTPROFILEMAPMODEL_H

#include "openPASS-AgentConfiguration/VehicleComponentProfileMapInterface.h"

class VehicleComponentProfileMapModel : public VehicleComponentProfileMapInterface
{
    Q_OBJECT

public:
    VehicleComponentProfileMapModel(QObject *parent = nullptr);
    ~VehicleComponentProfileMapModel() = default;

public:
    virtual VehicleComponentProfileMapInterface::Iterator begin() override;
    virtual VehicleComponentProfileMapInterface::ConstIterator constBegin() const override;

public:
    virtual VehicleComponentProfileMapInterface::Iterator end() override;
    virtual VehicleComponentProfileMapInterface::ConstIterator constEnd() const override;

public:
    virtual bool add(VehicleComponentProfileMapInterface::ID const & id) override;

public:
    virtual bool remove(VehicleComponentProfileMapInterface::ID const & id) override;
    virtual bool remove(VehicleComponentProfileMapInterface::Item * const item) override;

public:
    virtual VehicleComponentProfileMapInterface::Item * getItem(VehicleComponentProfileMapInterface::ID const & id) const override;

public:
    virtual void setID(VehicleComponentProfileMapInterface::Item  * const item,
                         VehicleComponentProfileMapInterface::ID const & id) override;

    virtual VehicleComponentProfileMapInterface::ID getID(VehicleComponentProfileMapInterface::Item  * const item) const override;


public:
    virtual bool contains(VehicleComponentProfileMapInterface::ID const & id) const override;
    virtual int count() const override;

public:
    virtual void clear() override;

public:
    virtual QList<ID> keys() const override;
    virtual QList<Item*> values() const override;

private:
    VehicleComponentProfileMapInterface::Map componentProfiles;
};

#endif // VEHICLECOMPONENTPROFILEMAPMODEL_H
