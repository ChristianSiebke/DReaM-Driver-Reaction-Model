/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once
#include "observedworldobject.h"

///
/// \brief The ObservedDynamicObject struct
///
struct ObservedDynamicObject : ObservedWorldObject {
    ///
    /// \brief Distance of the current Driver to the reference point of this object.
    ///
    double distanceToReference = std::numeric_limits<unsigned int>::max();

    ///
    /// \brief The velocity of this object in m/s.
    ///
    double velocity = std::numeric_limits<unsigned int>::max();

    ///
    /// \brief The acceleration of this object in m/s^2.
    ///
    double acceleration = std::numeric_limits<unsigned int>::max();
};
