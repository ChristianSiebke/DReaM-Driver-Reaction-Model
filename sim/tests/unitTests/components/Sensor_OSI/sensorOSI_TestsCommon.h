/********************************************************************************
 * Copyright (c) 2020 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "common/vector2d.h"
#include "osi3/osi_detectedobject.pb.h"

double constexpr EPSILON = 1e-9;

class MovingObjectParameter
{
public:
    MovingObjectParameter(unsigned int id,
                          Common::Vector2d position,
                          Common::Vector2d velocity,
                          Common::Vector2d acceleration,
                          double yaw) :
        id(id),
        position(position),
        velocity(velocity),
        acceleration(acceleration),
        yaw(yaw)
    {}

    MovingObjectParameter (const osi3::DetectedMovingObject& movingObject)
    {
        id = movingObject.header().ground_truth_id(0).value();
        position = Common::Vector2d(movingObject.base().position().x(), movingObject.base().position().y());
        velocity = Common::Vector2d(movingObject.base().velocity().x(), movingObject.base().velocity().y());
        acceleration = Common::Vector2d(movingObject.base().acceleration().x(), movingObject.base().acceleration().y());
        yaw = movingObject.base().orientation().yaw();
    }

    bool operator==(const MovingObjectParameter& rhs) const
    {
        if (id != rhs.id)
        {
            return false;
        }

        if (position != rhs.position ||
            velocity != rhs.velocity ||
            acceleration != rhs.acceleration)
        {
            return false;
        }

        if ((yaw - rhs.yaw) > EPSILON)
        {
            return false;
        }

        return true;
    }

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const MovingObjectParameter& obj)
    {
        os << "id: " << obj.id << ", "
           << "position: (" << obj.position.x << ", " << obj.position.y << "), "
           << "velocity: (" << obj.velocity.x << ", " << obj.velocity.y << "), "
           << "acceleration: (" << obj.acceleration.x << ", " << obj.acceleration.y << "), "
           << "yaw: " << obj.yaw;

        return os;
    }

    unsigned int id;
    Common::Vector2d position;
    Common::Vector2d velocity;
    Common::Vector2d acceleration;
    double yaw;
};

class StationaryObjectParameter
{
public:
    StationaryObjectParameter(unsigned int id,
                          Common::Vector2d position,
                          double yaw) :
        id(id),
        position(position),
        yaw(yaw)
    {}

    StationaryObjectParameter (const osi3::DetectedStationaryObject& stationaryObject)
    {
        id = stationaryObject.header().ground_truth_id(0).value();
        position = Common::Vector2d(stationaryObject.base().position().x(), stationaryObject.base().position().y());
        yaw = stationaryObject.base().orientation().yaw();
    }

    bool operator==(const StationaryObjectParameter& rhs)
    {
        if (id != rhs.id)
        {
            return false;
        }

        if (position != rhs.position)
        {
            return false;
        }

        if ((yaw - rhs.yaw) > EPSILON)
        {
            return false;
        }

        return true;
    }

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const StationaryObjectParameter& obj)
    {
        os << "id: " << obj.id << ", "
           << "position: (" << obj.position.x << ", " << obj.position.y << "), "
           << "yaw: " << obj.yaw;

        return os;
    }

    unsigned int id;
    Common::Vector2d position;
    double yaw;
};
