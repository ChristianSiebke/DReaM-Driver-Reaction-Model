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
#ifndef OBSERVEDDYNAMICOBJECT_H
#define OBSERVEDDYNAMICOBJECT_H

#include "observedworldobject.h"

///
/// \brief The ObservedDynamicObject struct
///
struct ObservedDynamicObject : ObservedWorldObject {
    ///
    /// \brief Distance of the current Driver to the reference point of this object.
    ///
    double distanceToReference;

    ///
    /// \brief The velocity of this object in m/s.
    ///
    double velocity;

    ///
    /// \brief The acceleration of this object in m/s^2.
    ///
    double acceleration;
};

#endif // OBSERVEDDYNAMICOBJECT_H
