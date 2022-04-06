/******************************************************************************
 * Copyright (c) 2020 TU Dresden
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
#pragma once
#include "common/Definitions.h"

namespace MentalInfrastructure {

///
/// \brief Represents an element in the road network.
///
class Element
{
public:
    Element(const OdId openDriveId, const DReaMId dreamId) : openDriveId(openDriveId), dreamId(dreamId) {
    }
    virtual ~Element()
    {
    }

    ///
    /// \brief Returns the OpenDrive id of this element.
    ///
    OdId GetOpenDriveId() const;

    DReaMId GetDReaMId() const;

protected:
    const OdId openDriveId;
    const DReaMId dreamId;
};
} // namespace MentalInfrastructure
