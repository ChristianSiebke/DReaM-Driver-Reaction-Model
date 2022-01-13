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
