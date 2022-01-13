#include "Road.h"

#include "Section.h"

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

const std::vector<const Section *> &Road::GetSections() const {
    return sections;
}

void Road::AddSection(const Section *section) {
    if (std::find(sections.begin(), sections.end(), section) == sections.end()) {
        sections.push_back(section);
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

} // namespace MentalInfrastructure
