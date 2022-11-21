/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Element.h"

namespace MentalInfrastructure {

OdId Element::GetOpenDriveId() const {
    return openDriveId;
}

DReaMId Element::GetDReaMId() const {
    return dreamId;
}

} // namespace MentalInfrastructure
