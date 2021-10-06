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
//! @file  ParameterItemInterface.h
//! @ingroup agentConfigurationPlugin
//! @brief  This class defines parameters.
//!
//!         Parameters consist of a key description, a datatype, a value and a
//!         unit. If parameters are registered in a map container class (here
//!         ParameterMapInterface), they also have a key identifier which can
//!         be accessed here. For simplicity, especially when bringing the
//!         parameters to an XML representation, all attributes are modeled
//!         as strings.
//-----------------------------------------------------------------------------
#ifndef PARAMETERITEMINTERFACE_H
#define PARAMETERITEMINTERFACE_H

#include <QObject>
#include <QString>

//-----------------------------------------------------------------------------
//! @brief  This class defines parameters.
//!
//!         Parameters consist of a key description, a datatype, a value and a
//!         unit. If parameters are registered in a map container class (here
//!         ParameterMapInterface), they also have a key identifier which can
//!         be accessed here. For simplicity, especially when bringing the
//!         parameters to an XML representation, all attributes are modeled
//!         as strings.
//-----------------------------------------------------------------------------
class ParameterItemInterface : public QObject
{
    Q_OBJECT

public:
    using ID = unsigned int; //!< typedef for the parameters id (when registered in a map)
    using Type = QString;    //!< typedef for the data type
    using Key = QString;     //!< typedef for the key description
    using Value = QString;   //!< typedef for the parameter's value
    using Unit = QString;    //!< typedef for the parameter's unit

public:
    ParameterItemInterface(QObject *const parent = nullptr) :
        QObject(parent)
    {
    }
    virtual ~ParameterItemInterface() = default;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the Id of this parameter if this parameter is registered
    //! in a map (ParameterMapInterface). Otherwise, an empty Id is returned.
    //!
    //! @return             the id of the parameter if registered in a map, otherwise
    //!                     an empty id
    //-----------------------------------------------------------------------------
    virtual ID getID() const = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the datatype of this parameter.
    //!
    //! @return             the datatype of this parameter
    //-----------------------------------------------------------------------------
    virtual Type getType() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the datatype of this parameter to the given type.
    //!
    //! @param[in]             the new datatype of this parameter
    //-----------------------------------------------------------------------------
    virtual void setType(Type const &type) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the key description of this parameter.
    //!
    //! @return             the key description of this parameter
    //-----------------------------------------------------------------------------
    virtual Key getKey() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the key description of this parameter to the given one.
    //!
    //! @param[in]             the new key description of this parameter
    //-----------------------------------------------------------------------------
    virtual void setKey(Key const &key) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the value of this parameter.
    //!
    //! @return             the value of this parameter
    //-----------------------------------------------------------------------------
    virtual Value getValue() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the value of this parameter to the given one.
    //!
    //! @param[in]             the new value of this parameter
    //-----------------------------------------------------------------------------
    virtual void setValue(Value const &value) = 0;

public:
    //-----------------------------------------------------------------------------
    //! Method which returns the unit of this parameter.
    //!
    //! @return             the unit of this parameter
    //-----------------------------------------------------------------------------
    virtual Unit getUnit() const = 0;

    //-----------------------------------------------------------------------------
    //! Method which sets the unit of this parameter to the given one.
    //!
    //! @param[in]             the new unit of this parameter
    //-----------------------------------------------------------------------------
    virtual void setUnit(Value const &unit) = 0;
};

#endif // PARAMETERITEMINTERFACE_H
