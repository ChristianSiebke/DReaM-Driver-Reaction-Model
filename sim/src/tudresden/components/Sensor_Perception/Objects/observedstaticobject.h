/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
