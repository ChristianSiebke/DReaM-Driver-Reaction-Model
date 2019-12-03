/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <iostream>

#include "trajectoryImporter.h"
#include "CoreFramework/CoreShare/xmlParser.h"
#include "CoreFramework/CoreShare/log.h"

namespace TAG = openpass::importer::xml::trajectoryImporter::tag;
namespace ATTRIBUTE = openpass::importer::xml::trajectoryImporter::attribute;

namespace Importer
{

bool TrajectoryImporter::Import(const std::string filename, TrajectoryInterface *trajectory)
{
    try
    {
        QFile xmlFile(filename.c_str()); // automatic object will be closed on destruction
        ThrowIfFalse(xmlFile.open(QIODevice::ReadOnly), "Could not open Trajectory.");

        QByteArray xmlData(xmlFile.readAll());
        QDomDocument document;
        ThrowIfFalse(document.setContent(xmlData), "Invalid xml format of Trajectories.");

        QDomElement documentRoot = document.documentElement();
        ThrowIfFalse(!documentRoot.isNull(), "Trajectory file was empty.");

        ImportTrajectory(documentRoot, trajectory);

        return true;
    }
    catch (const std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "Trajectory import failed: " + std::string(e.what());
        return false;
    }
}

void TrajectoryImporter::DetermineTrajectoryType(QDomElement trajectoryCoordinateElement, TrajectoryInterface *trajectory)
{
    double firstSCoord = -1;
    if (SimulationCommon::ParseAttributeDouble(trajectoryCoordinateElement, ATTRIBUTE::s, firstSCoord))
    {
        if(firstSCoord == 0.0)
        {
            trajectory->SetTrajectoryType(TrajectoryType::RoadCoordinatesRelative);
        }
        else
        {
            trajectory->SetTrajectoryType(TrajectoryType::RoadCoordinatesAbsolute);
        }
    }
    else if (trajectoryCoordinateElement.hasAttribute(QString::fromStdString("X")))
    {
        trajectory->SetTrajectoryType(TrajectoryType::WorldCoordinatesAbsolute);
    }
    else
    {
        LogErrorAndThrow("No valid trajectory type found.");
    }
}

void TrajectoryImporter::ImportTrajectory(QDomElement trajectoryElement, TrajectoryInterface *trajectory)
{
    QDomElement trajectoryCoordinateElement;
    ThrowIfFalse(SimulationCommon::GetFirstChildElement(trajectoryElement, TAG::trajectoryCoordinate, trajectoryCoordinateElement), "No trajectory coordinates found.");

    DetermineTrajectoryType(trajectoryCoordinateElement, trajectory);

    TrajectoryType type = trajectory->GetTrajectoryType();
    while(!trajectoryCoordinateElement.isNull())
    {
        int trajectoryCoordinateTime;
        ThrowIfFalse(SimulationCommon::ParseAttributeInt(trajectoryCoordinateElement, ATTRIBUTE::time, trajectoryCoordinateTime), "Trajectory coordinate must contain Time.");

        switch (type) {
        case TrajectoryType::RoadCoordinatesAbsolute:
        case TrajectoryType::RoadCoordinatesRelative:
        {
            RoadPosition roadCoordinate;

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(trajectoryCoordinateElement, ATTRIBUTE::s, roadCoordinate.s)
                            && SimulationCommon::ParseAttributeDouble(trajectoryCoordinateElement, ATTRIBUTE::t, roadCoordinate.t)
                            && SimulationCommon::ParseAttributeDouble(trajectoryCoordinateElement, ATTRIBUTE::hdg, roadCoordinate.hdg),
                         "Could not import road coordinate.");

            ThrowIfFalse(trajectory->AddRoadCoordinate(trajectoryCoordinateTime, roadCoordinate), "Trajectory coordinate time must be unique.");

            break;
        }
        case TrajectoryType::WorldCoordinatesAbsolute:
        {
            Position worldCoordinate;

            ThrowIfFalse(SimulationCommon::ParseAttributeDouble(trajectoryCoordinateElement, ATTRIBUTE::x, worldCoordinate.xPos)
                            && SimulationCommon::ParseAttributeDouble(trajectoryCoordinateElement, ATTRIBUTE::y, worldCoordinate.yPos)
                            && SimulationCommon::ParseAttributeDouble(trajectoryCoordinateElement, ATTRIBUTE::yaw, worldCoordinate.yawAngle),
                         "Could not import world coordinate.");

            ThrowIfFalse(trajectory->AddWorldCoordinate(trajectoryCoordinateTime, worldCoordinate), "Trajectory coordinate time must be unqiue.");

            break;
        }
        default:
            LogErrorAndThrow("Unknown trajectory type.");
        }
        trajectoryCoordinateElement = trajectoryCoordinateElement.nextSiblingElement("TrajectoryCoordinate");
    }
}

} //namespace Importer
