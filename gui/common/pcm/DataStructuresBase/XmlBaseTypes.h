/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef XMLBASETYPES_H
#define XMLBASETYPES_H

//-----------------------------------------------------------------------------
//! Enumeration class for the parameter types
//-----------------------------------------------------------------------------
enum class XML_PARAMETER_TYPE
{
    undefined = 0,
    bool_,
    int_,
    double_,
    string_,
    boolVector,
    intVector,
    doubleVector
};

//-----------------------------------------------------------------------------
//! Enumeration class for the component types
//-----------------------------------------------------------------------------
enum class XML_COMPONENT_TYPE
{
    undefined = 0,
    init,
    dynamics,
    other
};

//-----------------------------------------------------------------------------
//! Enumeration class for the agent types
//-----------------------------------------------------------------------------
enum class XML_AGENT_TYPE
{
    undefined = 0,
    pedestrian,
    car,
    other
};

#endif // XMLBASETYPES_H
