#include "Road.h"
#include "Section.h"

namespace MentalInfrastructure {

std::string Road::GetOpenDriveId() const { return openDriveId; }

Common::Vector2d Road::GetStartPosition() const { return startPos; }

double Road::GetLength() const { return roadLength; }

double Road::GetStartHeading() const { return roadHdg; }

///
/// Intersection data
///

bool Road::IsOnIntersection() const { return intersectionPtr != nullptr; }

void Road::SetOnIntersection(const Intersection* intersection) { intersectionPtr = intersection; }

const Intersection* Road::GetIntersection() const { return intersectionPtr; }

bool Road::IsPredecessorIntersection() const {
    auto testPtr = dynamic_cast<const Intersection*>(predecessor);
    return testPtr != nullptr;
}

bool Road::IsSuccessorIntersection() const {
    auto testPtr = dynamic_cast<const Intersection*>(successor);
    return testPtr != nullptr;
}

void Road::SetPredecessor(const Element* element) {
    auto testPtrA = dynamic_cast<const Intersection*>(element);
    auto testPtrB = dynamic_cast<const Road*>(element);

    if (testPtrA == nullptr && testPtrB == nullptr) {
        // element is not a road or intersection
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " Predecessor of a Road has to be a Road or Intersection!";
        throw std::runtime_error(message);
    }

    predecessor = element;
}

void Road::SetSuccessor(const Element* element) {
    auto testPtrA = dynamic_cast<const Intersection*>(element);
    auto testPtrB = dynamic_cast<const Road*>(element);

    if (testPtrA == nullptr && testPtrB == nullptr) {
        // element is not a road or intersection
        std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + " Successor of a Road has to be a Road or Intersection!";
        throw std::runtime_error(message);
    }
    successor = element;
}

const std::vector<const Section*>& Road::GetSections() const { return sections; }

void Road::AddSection(const Section* section) {
    if (std::find(sections.begin(), sections.end(), section) == sections.end()) {
        sections.push_back(section);
    }
}

const std::vector<const TrafficSign*>& Road::GetTrafficSigns() const { return trafficSigns; }

void Road::AddTrafficSign(const TrafficSign* sign) {
    if (std::find(trafficSigns.begin(), trafficSigns.end(), sign) == trafficSigns.end()) {
        trafficSigns.push_back(sign);
    }
}

} // namespace MentalInfrastructure
