/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef OBSERVEDTRAFFICSIGN_H
#define OBSERVEDTRAFFICSIGN_H

#include "common/Definitions.h"
#include "common/worldDefinitions.h"
#include "observedstaticobject.h"

///
/// \brief The ObservedTrafficSign struct
///
struct ObservedTrafficSign : ObservedStaticObject
{
    ///
    /// \brief The type of sign
    ///
    CommonTrafficSign::Type signType;
};

#endif // OBSERVEDTRAFFICSIGN_H
