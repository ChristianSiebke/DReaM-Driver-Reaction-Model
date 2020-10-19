
#include <sstream>
#include <include/signalInterface.h>

#include "openScenarioDefinitions.h"
class AcquirePositionSignal : public ComponentStateSignalInterface
{
public:
    static constexpr char COMPONENTNAME[] = "AcquirePositionSignal";

    AcquirePositionSignal(){
        componentState = ComponentState::Disabled;
    };
    AcquirePositionSignal(ComponentState componentState, openScenario::Position position) :
        position(std::move(position))
    {
        this->componentState = componentState;
    };
    AcquirePositionSignal(const AcquirePositionSignal &) = delete;
    AcquirePositionSignal(AcquirePositionSignal &&) = delete;

    ~AcquirePositionSignal() override = default;

    AcquirePositionSignal& operator=(const AcquirePositionSignal&) = delete;
    AcquirePositionSignal& operator=(AcquirePositionSignal&&) = delete;

    openScenario::Position position;

    explicit operator std::string() const override {
        std::ostringstream stream{};
        stream << COMPONENTNAME << "\n"
               << "openScenario::Position output stream operator not implemented.";
        return stream.str();
    };
};

