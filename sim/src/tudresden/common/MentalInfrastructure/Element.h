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
#include "common/Definitions.h"

namespace MentalInfrastructure {

///
/// \brief Represents an element in the road network.
///
class Element
{
public:
    Element(const OdId &openDriveId, const DReaMId dreamId) : openDriveId(openDriveId), dreamId(dreamId) {
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
