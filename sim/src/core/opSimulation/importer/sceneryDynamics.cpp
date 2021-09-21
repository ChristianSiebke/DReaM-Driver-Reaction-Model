#include "sceneryDynamics.h"

SceneryDynamics::SceneryDynamics()
{
}

openScenario::EnvironmentAction SceneryDynamics::GetEnvironment() const
{
    return environment;
}

void SceneryDynamics::SetEnvironment(const openScenario::EnvironmentAction &environment)
{
    this->environment = environment;
}

std::vector<openScenario::TrafficSignalController> SceneryDynamics::GetTrafficSignalControllers() const
{
    return trafficSignalControllers;
}

void SceneryDynamics::AddTrafficSignalController(const openScenario::TrafficSignalController &controller)
{
    trafficSignalControllers.push_back(controller);
}
