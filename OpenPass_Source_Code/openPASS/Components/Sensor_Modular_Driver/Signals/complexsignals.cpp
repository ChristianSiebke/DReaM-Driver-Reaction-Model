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
//! @file    complexsignals.cpp
//! @author  Christian Siebke
//! @author  Konstantin Blenz
//! @date    Tue, 03.12.2019
//! @brief provide signal interface for the sensor to the driver
//!
//-----------------------------------------------------------------------------
//! \addtogroup Algorithm_ModularDriver
//-----------------------------------------------------------------------------

#include "complexsignals.h"
#include <vector>
#include <string>
#include <sstream>
#include "ContainerStructures.h"

template<class T>
structSignal<T>:: operator std::string() const
{
    return std::to_string(value);
};

template<>
structSignal<int>::operator std::string() const
{
    std::ostringstream stream;
    stream<<value<<std::endl;

    return stream.str();
};

template<>
structSignal<std::list<SurroundingMovingObjectsData>>::operator std::string() const
{
    std::ostringstream stream;
    stream<<value.begin()->GetName()<<std::endl;

    return stream.str();
};

template<>
structSignal<StaticEnvironmentData>::operator std::string() const
{
    std::ostringstream stream;
    stream<<value.GetName()<<std::endl;

    return stream.str();
};

template<>
structSignal<egoData>::operator std::string() const
{
    std::ostringstream stream;
    stream<<value.GetName()<<std::endl;

    return stream.str();
};




