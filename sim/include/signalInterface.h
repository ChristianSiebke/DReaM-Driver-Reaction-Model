/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <map>
#include <string>

enum class ComponentState
{
    Undefined = 0,
    Disabled,
    Armed,
    Acting
};

namespace openpass::utils {

/// @brief constexpr map for transforming the a corresponding enumeration into
///        a string representation: try to_cstr(EnumType) or to_string(EnumType)
static constexpr std::array<const char *, 4> ComponentStateMapping{
    "Undefined",
    "Disabled",
    "Armed",
    "Acting"};

constexpr const char *to_cstr(ComponentState state)
{
    return ComponentStateMapping[static_cast<size_t>(state)];
}

inline std::string to_string(ComponentState state) noexcept
{
    return std::string(to_cstr(state));
}

} // namespace openpass::utils

const std::map<std::string, ComponentState> ComponentStateMapping = {{"Acting", ComponentState::Acting},
                                                                     {"Armed", ComponentState::Armed},
                                                                     {"Disabled", ComponentState::Disabled}};

enum class ComponentWarningLevel
{
    INFO = 0,
    WARNING
};

const std::map<ComponentWarningLevel, std::string> ComponentWarningLevelMapping = {{ComponentWarningLevel::INFO, "Info"},
                                                                                   {ComponentWarningLevel::WARNING, "Warning"}};

enum class ComponentWarningType
{
    OPTIC = 0,
    ACOUSTIC,
    HAPTIC
};

const std::map<ComponentWarningType, std::string> ComponentWarningTypeMapping = {{ComponentWarningType::OPTIC, "Optic"},
                                                                                 {ComponentWarningType::ACOUSTIC, "Acoustic"},
                                                                                 {ComponentWarningType::HAPTIC, "Haptic"}};

enum class ComponentWarningIntensity
{
    LOW = 0,
    MEDIUM,
    HIGH
};

const std::map<ComponentWarningIntensity, std::string> ComponentWarningIntensityMapping = {{ComponentWarningIntensity::LOW, "Low"},
                                                                                           {ComponentWarningIntensity::MEDIUM, "Medium"},
                                                                                           {ComponentWarningIntensity::HIGH, "High"}};

struct ComponentWarningInformation
{
    bool activity;
    ComponentWarningLevel level;
    ComponentWarningType type;
    ComponentWarningIntensity intensity;
};

//-----------------------------------------------------------------------------
//! This interface provides access to common signal operations
//-----------------------------------------------------------------------------
class SignalInterface
{
public:
    SignalInterface() = default;
    SignalInterface(const SignalInterface &) = default;
    SignalInterface(SignalInterface &&) = default;
    SignalInterface &operator=(const SignalInterface &) = default;
    SignalInterface &operator=(SignalInterface &&) = default;
    virtual ~SignalInterface() = default;

    //-----------------------------------------------------------------------------
    //! String conversion of signal
    //-----------------------------------------------------------------------------
    explicit virtual operator std::string() const = 0;

private:
    friend std::ostream& operator<<(std::ostream&, const SignalInterface&);
};

//-----------------------------------------------------------------------------
//! Provides functionality to print information of signals
//!
//! @param[in]     stream  Output stream for printing information
//! @param[in]     signal  Signal to be printed
//! @return                Output stream for concatenation
//-----------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& stream, const SignalInterface& signal)
{
    return stream << static_cast<std::string>(signal);
}

//-----------------------------------------------------------------------------
//! This interface provides access to a component's state
//-----------------------------------------------------------------------------
class ComponentStateSignalInterface : public SignalInterface
{
public:
    ComponentStateSignalInterface() = default;
    ComponentStateSignalInterface(ComponentState componentState) :
        componentState{componentState}
    {
    }
    ComponentStateSignalInterface(const ComponentStateSignalInterface &) = default;
    ComponentStateSignalInterface(ComponentStateSignalInterface &&) = default;
    ComponentStateSignalInterface &operator=(const ComponentStateSignalInterface &) = default;
    ComponentStateSignalInterface &operator=(ComponentStateSignalInterface &&) = default;
    virtual ~ComponentStateSignalInterface() = default;

    ComponentState componentState;
};
