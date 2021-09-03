#pragma once

class FmuWrapperInterface
{
public:
    virtual void Init() = 0;
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<const SignalInterface> &data, int time) = 0;
    virtual void Trigger(int time) = 0;
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<const SignalInterface> &data, int time) = 0;
    [[nodiscard]] virtual const FmuHandlerInterface *GetFmuHandler() const = 0;
    [[nodiscard]] virtual const FmuVariables &GetFmuVariables() const = 0;
    [[nodiscard]] virtual const fmu_check_data_t &GetCData() const = 0;
    [[nodiscard]] virtual const FmuHandlerInterface::FmuValue& GetValue(fmi2_value_reference_t valueReference, VariableType variableType) const = 0;
};
