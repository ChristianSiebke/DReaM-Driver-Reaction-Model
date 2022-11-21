/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef SENSORPROFILEPARAMETERITEMINTERFACE_H
#define SENSORPROFILEPARAMETERITEMINTERFACE_H

#include <QObject>

class SensorProfileParameterItemInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int;
    using Type = QString;
    using Key = QString;
    using Value = QString;

public:
    SensorProfileParameterItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~SensorProfileParameterItemInterface() = default;

public:
    virtual ID getID() const = 0;

public:
    virtual Type getType() const = 0;
    virtual void setType(Type const &type) = 0;

public:
    virtual Key getKey() const = 0;
    virtual void setKey(Key const &key) = 0;

public:
    virtual Value getValue() const = 0;
    virtual void setValue(Value const &value) = 0;
};

#endif // SENSORPROFILEPARAMETERITEMINTERFACE_H
