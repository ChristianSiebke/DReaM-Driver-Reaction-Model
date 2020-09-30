/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include <memory>
#include <qglobal.h>
#include "dynamics_copyTrajectory_implementation.h"

Dynamics_CopyTrajectory_Implementation::Dynamics_CopyTrajectory_Implementation(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface *const publisher,
        const CallbackInterface *callbacks,
        AgentInterface *agent) :
    DynamicsInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent)
{
    LOGINFO(QString().sprintf("Constructing Dynamics_CopyTrajectory for agent %d...", agent->GetId()).toStdString());
    timeStep = (double)cycleTime / 1000.0;
    LOGINFO("Constructing Dynamics_CopyTrajectory successful");
}

void Dynamics_CopyTrajectory_Implementation::UpdateInput(int localLinkId,
                                                         const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);
    Q_UNUSED(localLinkId);

    LOGDEBUG(QString().sprintf("%s UpdateInput", COMPONENTNAME.c_str()).toStdString());


    //    bool success = false;
    const auto trajectorySignal = std::dynamic_pointer_cast<TrajectorySignal const>(data);

    //success = inputPorts.at(localLinkId)->SetSignalValue(trajectorySignal);

    try
    {
        trajectory = trajectorySignal->trajectory;
        ReadWayPointData();
        LOGDEBUG(QString().sprintf("%s UpdateInput successful", COMPONENTNAME.c_str()).toStdString());
    }
    catch (...)
    {
        LOGERROR(QString().sprintf("%s UpdateInput failed", COMPONENTNAME.c_str()).toStdString());
    }

}

void Dynamics_CopyTrajectory_Implementation::UpdateOutput(int localLinkId,
                                                          std::shared_ptr<SignalInterface const> &data, int time)
{
    // no outputs of the module
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);
}

void Dynamics_CopyTrajectory_Implementation::Trigger(int time)
{

    //    LOGDEBUG("Triggering Dynamics_CopyTrajectory...");

    double timeSec = (double)time / 1000.0;
    double x, y, vx, vy, yaw, w;

    if (timeSec <= timeVec.front())
    {
        x = posX.front();
        y = posY.front();
        vx = velX.front();
        vy = velY.front();
        yaw = angleYaw.front();
        w = rateYaw.front();
    }
    else if (timeSec >= timeVec.back())
    {
        x = posX.back();
        y = posY.back();
        vx = velX.back();
        vy = velY.back();
        yaw = angleYaw.back();
        w = rateYaw.back();
    }
    else
    {
        while (timeSec > timeVecNext)
        {
            indexVecNext++;
            timeVecNext = timeVec[indexVecNext];
        }
        unsigned int indexVecPassed = indexVecNext-1;
        double dT = (timeSec - timeVec[indexVecPassed]);
        if (dT<1e-6 && dT>-1e-6)
        {
            dT = 0.0;
        }
        x = posX[indexVecPassed] + velX[indexVecPassed] * dT;
        y = posY[indexVecPassed] + velY[indexVecPassed] * dT;
        yaw = angleYaw[indexVecPassed] + rateYaw[indexVecPassed] * dT;
        vx = velX[indexVecPassed];
        vy = velY[indexVecPassed];
        w = rateYaw[indexVecPassed];

        //        LOGDEBUG(QString().sprintf(
        //                     "t = %f; iPassed = %d; iNext = %d",
        //                     timeSec, indexVecPassed, indexVecNext).toStdString());

    }

    //    LOGDEBUG(QString().sprintf(
    //                 "x = %f; y = %f; yaw = %f; vx = %f; vy = %f; w = %f",
    //                 x, y, yaw, vx, vy, w).toStdString());

    GetAgent()->SetPositionX(x);
    GetAgent()->SetPositionY(y);
    GetAgent()->SetVelocityVector(vx, vy, 0.0);
    GetAgent()->SetYaw(yaw);
    GetAgent()->SetYawRate(w);

    //    LOGDEBUG("Trigger Dynamics_CopyTrajectory executed!");
}

void Dynamics_CopyTrajectory_Implementation::ReadWayPointData()
{
    unsigned int n = trajectory.points.size();

    double vX = 0.0;
    double vY = 0.0;
    double w = 0.0;

    timeVec.resize(trajectory.points.size());
    posX.resize(trajectory.points.size());
    posY.resize(trajectory.points.size());
    velX.resize(trajectory.points.size());
    velY.resize(trajectory.points.size());
    angleYaw.resize(trajectory.points.size());
    rateYaw.resize(trajectory.points.size());

    for (unsigned int i = 0; i < n; ++i)
    {
        openScenario::TrajectoryPoint point = trajectory.points.at(i);

        timeVec[i] = point.time;
        posX[i] = point.x;
        posY[i] = point.y;
        angleYaw[i] = point.yaw;
        if (i<n-1)
        {
            openScenario::TrajectoryPoint pointNext = trajectory.points.at(i+1);
            vX = (pointNext.x-point.x) / timeStep; // uniform motion approximation
            vY = (pointNext.y-point.y) / timeStep; // uniform motion approximation
            w = (pointNext.yaw-point.yaw) / timeStep; // uniform motion approximation
        }
        velX[i] = vX;
        velY[i] = vY;
        rateYaw[i] = w;
    }

    indexVecNext = 1;
    timeVecNext = timeVec[indexVecNext];

}
