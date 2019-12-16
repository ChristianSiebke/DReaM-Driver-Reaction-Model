/****************************************************************************** 
* Copyright (c) 2017 Volkswagen Group of America. 
* 
* This program and the accompanying materials are made 
* available under the terms of the Eclipse Public License 2.0 
* which is available at https://www.eclipse.org/legal/epl-2.0/ 
* 
* SPDX-License-Identifier: EPL-2.0 
******************************************************************************/ 

/*
//-----------------------------------------------------------------------------
//! @file ComponentPlugin.cpp
//! @defgroup   componentPlugin Component Plugin
//! @brief The Component Plugin (CP) is dedicated for modelling components.
//!
//! \htmlonly
//! <a name="dependencies" id="dependencies"></a><h2 class="groupheader">Dependencies</h2>
//! \endhtmlonly
//!
//! \image html plugin_component__inherit__graph.svg
//!
//! \section componentPluginDef Component Plugin
//!
//! @details The Component Plugin (CP) is dedicated for modelling components. It integrates
//! the definition structure of a component.
//!
//! \subsection component Component Definition
//! A component is defined with the following items:
//! - a key name as unique identifier for referencing (filename),
//! - a type which can be either Action or Algorithm or Sensor,
//! - a title to enable the user give other names to the component,
//! - a parameter set \e "schedule" which control the execution of the component algorithm during a simulation,
//! - a set of parameters,
//! - a set of inputs,
//! - a set of outputs
//!
//!
//-----------------------------------------------------------------------------
*/

#include "ComponentPlugin.h"

#include "Models/ComponentModel.h"

ComponentPlugin::ComponentPlugin(QObject * const parent)
    : QObject(parent)
{
}

bool ComponentPlugin::initialize()
{
    componentModel = new ComponentModel();
    return true;
}

bool ComponentPlugin::deinitialize()
{
    delete componentModel;
    return true;
}

