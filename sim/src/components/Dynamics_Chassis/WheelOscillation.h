/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef WHEELOSCILLATION_H
#define WHEELOSCILLATION_H

//! Wheel force perpendicular.
class WheelOscillation
{
public:
    WheelOscillation() = default;
    WheelOscillation(int wid, double k, double q);
    WheelOscillation(const WheelOscillation &) = delete;
    WheelOscillation(WheelOscillation &&) = delete;
    WheelOscillation &operator=(const WheelOscillation &) = delete;
    WheelOscillation &operator=(WheelOscillation &&) = delete;

    virtual ~WheelOscillation() = default;

    void Init(int id, double time_step, double k, double q);

    //! Perform simulation;
    void Perform(double forceZ, double mass);

    double GetCurZPos() const { return curZ; }

private:
    int id = -1;
    double prevVz = 0.0;

    double curAz = 0.0;
    double curVz = 0.0;
    double curZ = 0.0;

    double maxZ = 0.01;
    double minZ = -0.04;

    double timeStep; // 0.01 second = 10 millisecond
    double coeffSpring;
    double coeffDamp = 2000.0;
};

#endif //WHEELOSCILLATION_H
