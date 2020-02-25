/******************************************************************************
* Copyright (c) 2019 TU Dresden
* Copyright (c) 2019 AMFD GmbH
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*****************************************************************************/

//-----------------------------------------------------------------------------
//! @file    complexsignals.h
//! @author  Christian Siebke
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide signal interface for the sensor to the driver
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#pragma once

#include "modelInterface.h"
#include <list>



template<class T>
class structSignal :public SignalInterface
{
public:
    structSignal(T inValue) : value(inValue)
    {}
    structSignal(const structSignal &) = default;
    structSignal(structSignal &&) = default;
    structSignal &operator=(const structSignal &) = default;
    structSignal &operator=(structSignal &&) = default;
    virtual ~structSignal() = default;

    //-----------------------------------------------------------------------------
    //! Converts signal to string
    //-----------------------------------------------------------------------------

    virtual operator std::string() const;
    //{
    //    return std::to_string(value);
    //}

    T value; //!< signal content

};


