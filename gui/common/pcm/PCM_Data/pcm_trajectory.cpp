/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "pcm_trajectory.h"
#include <cmath>

PCM_Trajectory::PCM_Trajectory(std::vector<double> *timeVec, std::vector<double> *xPosVec, std::vector<double> *yPosVec, std::vector<double> *uVelVec, std::vector<double> *vVelVec, std::vector<double> *psiVec) :
    timeVec(timeVec),
    xPosVec(xPosVec),
    yPosVec(yPosVec),
    uVelVec(uVelVec),
    vVelVec(vVelVec),
    psiVec(psiVec)
{
}

PCM_Trajectory::~PCM_Trajectory()
{
    Clear();
}

double PCM_Trajectory::GetEndTime() const
{
    return timeVec->back();
}

double PCM_Trajectory::GetStartVelocity() const
{
    return uVelVec->front();
}

std::vector<double> *PCM_Trajectory::GetTimeVec() const
{
    return timeVec;
}

std::vector<double> *PCM_Trajectory::GetXPosVec() const
{
    return xPosVec;
}

std::vector<double> *PCM_Trajectory::GetYPosVec() const
{
    return yPosVec;
}

std::vector<double> *PCM_Trajectory::GetUVelVec() const
{
    return uVelVec;
}

std::vector<double> *PCM_Trajectory::GetVVelVec() const
{
    return vVelVec;
}

std::vector<double> *PCM_Trajectory::GetPsiVec() const
{
    return psiVec;
}


QString PCM_Trajectory::GetTimeVecString() const
{
    return VecDoubleToCsv(timeVec);
}

QString PCM_Trajectory::GetXPosVecString() const
{
    return VecDoubleToCsv(xPosVec);
}

QString PCM_Trajectory::GetYPosVecString() const
{
    return VecDoubleToCsv(yPosVec);
}

QString PCM_Trajectory::GetUVelVecString() const
{
    return VecDoubleToCsv(uVelVec);
}

QString PCM_Trajectory::GetVVelVecString() const
{
    return VecDoubleToCsv(vVelVec);
}

QString PCM_Trajectory::GetPsiVecString() const
{
    return VecDoubleToCsv(psiVec);
}

QString PCM_Trajectory::VecIntToCsv(const std::vector<int> *inVec) const
{
    QString outString = "";

    if (!inVec->empty())
    {
        std::ostringstream oss;

        // Convert all but the last element to avoid a trailing ","
        std::copy(inVec->begin(), inVec->end()-1, std::ostream_iterator<int>(oss, ", "));

        // Now add the last element with no delimiter
        oss << inVec->back();
        outString = QString::fromStdString(oss.str());
    }


    return outString;
}

QString PCM_Trajectory::VecDoubleToCsv(const std::vector<double> *inVec) const
{
    QString outString = "";

    for (uint index = 0; index < inVec->size() - 1; index++)
    {
        outString += (QString::number(inVec->at(index), 'g', 7) + ", ");  // output precision up to 10 digits
    }

    outString += QString::number(inVec->at(inVec->size() - 1), 'g', 7);

    return outString;
}

void PCM_Trajectory::ShiftForward(double distance) const
{
    for (unsigned int i = 0; i < timeVec->size(); ++i)
    {
        xPosVec->at(i) += distance * std::cos(psiVec->at(i));
        yPosVec->at(i) += distance * std::sin(psiVec->at(i));
    }
}

void PCM_Trajectory::ShiftPosition(double distance, double angle) const
{    
    for (unsigned int i = 0; i < timeVec->size(); ++i)
    {
        xPosVec->at(i) += distance * std::cos(angle);
        yPosVec->at(i) += distance * std::sin(angle);
    }
}

void PCM_Trajectory::ScaleVelocity(double velScale) const
{
    for (unsigned int i = 0; i < timeVec->size(); ++i)
    {
        vVelVec->at(i) *= velScale;
        uVelVec->at(i) += velScale;

        if (i > 0)
        {
            double dt = static_cast<double>(timeVec->at(i) - timeVec->at(i - 1));
            double psi = psiVec->at(i);
            double vx = uVelVec->at(i - 1) * std::cos(psi) - vVelVec->at(i - 1) * std::sin(psi);
            double vy = uVelVec->at(i - 1) * std::sin(psi) + vVelVec->at(i - 1) * std::cos(psi);

            xPosVec->at(i) = xPosVec->at(i - 1) + dt * vx;
            yPosVec->at(i) = yPosVec->at(i - 1) + dt * vy;
        }
    }
}

void PCM_Trajectory::ResetToOriginalState()
{
    Clear();

    timeVec = new std::vector<double>(timeVecOriginal);
    xPosVec = new std::vector<double>(xPosVecOriginal);
    yPosVec = new std::vector<double>(yPosVecOriginal);
    uVelVec = new std::vector<double>(uVelVecOriginal);
    vVelVec = new std::vector<double>(vVelVecOriginal);
    psiVec = new std::vector<double>(psiVecOriginal);
}

void PCM_Trajectory::SaveOriginalState()
{
    timeVecOriginal = *timeVec;
    xPosVecOriginal = *xPosVec;
    yPosVecOriginal = *yPosVec;
    uVelVecOriginal = *uVelVec;
    vVelVecOriginal = *vVelVec;
    psiVecOriginal = *psiVec;
}

void PCM_Trajectory::Clear()
{
    if (timeVec != nullptr)
    {
        delete timeVec ;
        timeVec = nullptr;
    }
    if (xPosVec != nullptr)
    {
        delete xPosVec ;
        xPosVec = nullptr;
    }
    if (yPosVec != nullptr)
    {
        delete yPosVec ;
        yPosVec = nullptr;
    }
    if (uVelVec != nullptr)
    {
        delete uVelVec ;
        uVelVec = nullptr;
    }
    if (vVelVec != nullptr)
    {
        delete vVelVec ;
        vVelVec = nullptr;
    }
    if (psiVec != nullptr)
    {
        delete psiVec ;
        psiVec = nullptr;
    }
}
