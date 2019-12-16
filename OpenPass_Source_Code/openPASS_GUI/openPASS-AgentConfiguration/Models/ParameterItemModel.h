/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

#ifndef PARAMETERITEMMODEL_H
#define PARAMETERITEMMODEL_H

#include "openPASS-AgentConfiguration/ParameterItemInterface.h"

class ParameterItemModel : public ParameterItemInterface
{

    Q_OBJECT

public:
    ParameterItemModel(QObject *parent=nullptr);
    ParameterItemModel(ParameterItemInterface const * const parameter,
                       QObject *parent = nullptr);
    virtual ~ParameterItemModel() override = default;

public:
    virtual ID getID() const override;

public:
    virtual Type getType() const override;
    virtual void setType(Type const &_type) override;

public:
    virtual Key getKey() const override;
    virtual void setKey(Key const &_key) override;

public:
    virtual Value getValue() const override;
    virtual void setValue(Value const &_value) override;

public:
    virtual Unit getUnit() const override;
    virtual void setUnit(Value const &_unit) override;

private:
    static Type default_type;
    static Key default_key;
    static Value default_value;
    static Unit default_unit;

private:
    Type type;
    Key key;
    Value value;
    Unit unit;
};

#endif // PARAMETERITEMMODEL_H
