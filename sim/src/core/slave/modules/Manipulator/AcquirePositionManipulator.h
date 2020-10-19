
#pragma once

#include <src/common/openScenarioDefinitions.h>

#include "ManipulatorCommonBase.h"

class AcquirePositionManipulator : public ManipulatorCommonBase
{
public:
    AcquirePositionManipulator(WorldInterface *world,
                               SimulationSlave::EventNetworkInterface *eventNetwork,
                               const CallbackInterface *callbacks,
                               const std::string &eventName,
                               openScenario::AcquirePositionAction);

    /*!
    * \brief Triggers the functionality of this class
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    void Trigger(int time) override;

private:
    openScenario::AcquirePositionAction acquirePositionAction;
    EventContainer GetEvents() override;
};

