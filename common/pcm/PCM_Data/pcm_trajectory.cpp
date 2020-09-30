/*********************************************************************
* Copyright (c) 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "pcm_trajectory.h"
#include <cmath>

int PCM_Trajectory::GetEndTime()
{
    return timeVec->back();
}

double PCM_Trajectory::GetStartVelocity()
{
    return uVelVec->front();
}

std::vector<int> *PCM_Trajectory::GetTimeVec() const
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
    return VecIntToCsv(timeVec);
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

PCM_Trajectory* PCM_Trajectory::Copy(const PCM_Trajectory* trajectory)
{

    size_t sizeOfTrajectory = trajectory->GetTimeVec()->size();

    std::vector<int> *timeVec1 = new std::vector<int>;
    std::vector<double> *xPosVec1 = new std::vector<double>;
    std::vector<double> *yPosVec1 = new std::vector<double>;
    std::vector<double> *uVelVec1 = new std::vector<double>;
    std::vector<double> *vVelVec1 = new std::vector<double>;
    std::vector<double> *psiVec1 = new std::vector<double>;

    for (size_t it = 0; it<sizeOfTrajectory; ++it)
    {
        timeVec1->push_back(trajectory->GetTimeVec()->at(it));
        xPosVec1->push_back(trajectory->GetXPosVec()->at(it));
        yPosVec1->push_back(trajectory->GetYPosVec()->at(it));
        uVelVec1->push_back(trajectory->GetUVelVec()->at(it));
        vVelVec1->push_back(trajectory->GetVVelVec()->at(it));
        psiVec1->push_back(trajectory->GetPsiVec()->at(it));
    }

    return new PCM_Trajectory(timeVec1,
                              xPosVec1,
                              yPosVec1,
                              uVelVec1,
                              vVelVec1,
                              psiVec1);

}

void PCM_Trajectory::ReReference(PCM_ParticipantData agent)
{
    for (unsigned int i = 0; i < timeVec->size(); ++i)
    {
        double distanceRearAxleToCOG = agent.GetWheelbase().toDouble() - agent.GetDistcgfa().toDouble(); // > 0
        xPosVec->at(i) -= distanceRearAxleToCOG*std::cos(psiVec->at(i));
        yPosVec->at(i) -= distanceRearAxleToCOG*std::sin(psiVec->at(i));
    }
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
