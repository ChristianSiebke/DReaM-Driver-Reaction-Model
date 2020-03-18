/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "Interfaces/roadInterface/roadInterface.h"
#include "Interfaces/roadInterface/roadSignalInterface.h"

//-----------------------------------------------------------------------------
//! Class representing a road signal
//-----------------------------------------------------------------------------
class RoadSignal : public RoadSignalInterface
{
public:
    RoadSignal(RoadInterface *road, const RoadSignalSpecification signal) :
        road{road},
        signal{signal}
    {}

    //-----------------------------------------------------------------------------
    /// @brief Returns the type of the signal (e.g. code according to StVO)
    /// @return type
    //-----------------------------------------------------------------------------
    std::string GetType() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the type of the signal (e.g. code according to StVO)
    /// @return type
    //-----------------------------------------------------------------------------
    std::string GetSubType() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the unique identification string of the signal
    /// @return id
    //-----------------------------------------------------------------------------
    virtual std::string GetId() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the value
    /// @return value
    //-----------------------------------------------------------------------------
    double GetValue() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the unit
    /// @return unit
    //-----------------------------------------------------------------------------
    RoadSignalUnit GetUnit() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the text on the signal
    /// @return text
    //-----------------------------------------------------------------------------
    std::string GetText() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the s coordinate of the signal
    ///
    /// @return s [m]
    //-----------------------------------------------------------------------------
    double GetS() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the t coordinate of the signal
    ///
    /// @return s [m]
    //-----------------------------------------------------------------------------
    double GetT() const override;

    //-----------------------------------------------------------------------------
    //! Returns the road from which this section is a part of.
    //!
    //! @return                         Road from which this section is a part of
    //-----------------------------------------------------------------------------
    RoadInterface *GetRoad()
    {
        return road;
    }

    //-----------------------------------------------------------------------------
    //! Returns if the signal is valid for a given lane
    //! @param[in] laneId   lane Id of interest
    //!
    //! @return             true if signal is valid
    //-----------------------------------------------------------------------------
    bool IsValidForLane(int laneId) const override;


    //-----------------------------------------------------------------------------
    /// @brief Returns the height of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetHeight() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the width of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetWidth() const override;

    //-----------------------------------------------------------------------------
    /// @brief Returns the length of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetLength() const;

    //-----------------------------------------------------------------------------
    /// @brief Returns the pitch of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetPitch() const;
    //-----------------------------------------------------------------------------
    /// @brief Returns the roll of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetRoll() const;

    //-----------------------------------------------------------------------------
    /// @brief Returns the yaw of the signal
    ///
    /// @return height [m]
    //-----------------------------------------------------------------------------
    virtual double GetYaw() const;

    //-----------------------------------------------------------------------------
    /// @brief Returns true if the signal is dynamic
    ///
    /// @return true if dynamic == "yes"
    //-----------------------------------------------------------------------------
    virtual bool GetIsDynamic() const override;

    virtual std::list<std::string> GetDependencies() const override;

    virtual double GetZOffset() const override;

    virtual bool GetOrientation() const override;

    virtual double GetHOffset() const override;

private:
    RoadInterface* road;
    const RoadSignalSpecification signal;

public:
    RoadSignal(const RoadSignal&) = delete;
    RoadSignal(RoadSignal&&) = delete;
    RoadSignal& operator=(const RoadSignal&) = delete;
    RoadSignal& operator=(RoadSignal&&) = delete;
    virtual ~RoadSignal() override = default;
};
