/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ParameterItemModel.h
//! @ingroup agentConfigurationPlugin
//! @brief This class implements the functionality of ParameterItemInterface.
//-----------------------------------------------------------------------------

#ifndef PARAMETERITEMMODEL_H
#define PARAMETERITEMMODEL_H

#include "openPASS-AgentConfiguration/ParameterItemInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class implements the functionality of ParameterItemInterface.
//-----------------------------------------------------------------------------
class ParameterItemModel : public ParameterItemInterface
{
    Q_OBJECT

public:
    ParameterItemModel(QObject *parent = nullptr);
    ParameterItemModel(ParameterItemInterface const *const parameter,
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
    Type type;   //!< the string object for the parameter's datatype
    Key key;     //!< the string object  for the parameter's description
    Value value; //!< the parameter's value. Taken as a string in order to make it independent its datatype
    Unit unit;   //!< the string object for the parameter's unit

private:
    static Type default_type;   //!< the default datatype
    static Key default_key;     //!< the default key desription
    static Value default_value; //!< the default value
    static Unit default_unit;   //!< the default unit
};

#endif // PARAMETERITEMMODEL_H
