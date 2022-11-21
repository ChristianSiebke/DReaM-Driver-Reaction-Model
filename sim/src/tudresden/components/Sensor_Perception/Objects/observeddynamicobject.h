/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
