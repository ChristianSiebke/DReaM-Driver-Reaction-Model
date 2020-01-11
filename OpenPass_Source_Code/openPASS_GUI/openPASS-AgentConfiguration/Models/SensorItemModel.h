/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef SENSORITEMMODEL_H
#define SENSORITEMMODEL_H

#include "openPASS-AgentConfiguration/SensorItemInterface.h"

class SensorItemModel : public SensorItemInterface
{
public:
    SensorItemModel(QObject * parent = nullptr);
    ~SensorItemModel() = default;

public:
    virtual SensorPosition getSensorPosition() const override;
    virtual void setSensorPosition(SensorPosition const & _position) override;

public:
    virtual Profile * getSensorProfile() const override;
    virtual void setSensorProfile(Profile  * const _profile) override;

public:
    virtual ID getID() const override;

private:
    static SensorPosition default_position;
    SensorPosition position;
    Profile * profile;

};

#endif // SENSORITEMMODEL_H
