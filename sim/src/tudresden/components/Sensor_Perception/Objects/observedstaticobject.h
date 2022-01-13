/******************************************************************************
 * Copyright (c) 2021 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
#ifndef OBSERVEDSTATICOBJECT_H
#define OBSERVEDSTATICOBJECT_H

#include "include/roadInterface/roadElementTypes.h"
#include "observedworldobject.h"

///
/// \brief The ObservedStaticObject struct
///
struct ObservedStaticObject : ObservedWorldObject
{
    ///
    /// \brief The type of this object.
    ///
    RoadObjectType type;
};

#endif // OBSERVEDSTATICOBJECT_H
