/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Road.h"

#include "Lane.h"

namespace MentalInfrastructure {

Common::Vector2d Road::GetStartPosition() const {
    return startPos;
}

double Road::GetLength() const {
    return roadLength;
}

double Road::GetStartHeading() const {
    return roadHdg;
}

///
/// Junction data
///

bool Road::IsOnJunction() const {
    return junctionPtr != nullptr;
}

void Road::SetOnJunction(const Junction *junction) {
    junctionPtr = junction;
}

const Junction *Road::GetJunction() const {
    return junctionPtr;
}

bool Road::IsPredecessorJunction() const {
    auto testPtr = dynamic_cast<const Junction *>(predecessor);
    return testPtr != nullptr;
}

bool Road::IsSuccessorJunction() const {
    auto testPtr = dynamic_cast<const Junction *>(successor);
    return testPtr != nullptr;
}

void Road::SetPredecessor(const Element *element) {
    auto testPtrA = dynamic_cast<const Junction *>(element);
    auto testPtrB = dynamic_cast<const Road *>(element);

    if (testPtrA == nullptr && testPtrB == nullptr) {
        // element is not a road or junction
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " Predecessor of a Road has to be a Road or Junction!";
        throw std::runtime_error(message);
    }

    predecessor = element;
}

void Road::SetSuccessor(const Element *element) {
    auto testPtrA = dynamic_cast<const Junction *>(element);
    auto testPtrB = dynamic_cast<const Road *>(element);

    if (testPtrA == nullptr && testPtrB == nullptr) {
        // element is not a road or junction
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " Successor of a Road has to be a Road or Junction!";
        throw std::runtime_error(message);
    }
    successor = element;
}

const std::vector<const Lane *> &Road::GetLanes() const {
    return lanes;
}

void Road::AddLane(const Lane *lane) {
    if (std::find(lanes.begin(), lanes.end(), lane) == lanes.end()) {
        lanes.push_back(lane);
    }
}

const std::vector<const TrafficSign *> &Road::GetTrafficSigns() const {
    return trafficSigns;
}

void Road::AddTrafficSign(const TrafficSign *sign) {
    if (std::find(trafficSigns.begin(), trafficSigns.end(), sign) == trafficSigns.end()) {
        trafficSigns.push_back(sign);
    }
}

void Road::AddTrafficLight(const TrafficLight *trafficLight) {
    if (std::find(trafficLights.begin(), trafficLights.end(), trafficLight) == trafficLights.end()) {
        trafficLights.push_back(trafficLight);
    }
}

} // namespace MentalInfrastructure
