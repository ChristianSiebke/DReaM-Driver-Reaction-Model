/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "vehicleModelImporter.h"
#include "pcm_definitions.h"

bool VehicleModelImporter::Import(const std::string &filename,
                                  std::map<int, PCM_ParticipantData *> &particpants)
{
    std::locale::global(std::locale("C"));

    QFile xmlFile(filename.c_str()); // automatic object will be closed on destruction
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QByteArray xmlData(xmlFile.readAll());
    QDomDocument document;
    QString errorMsg;
    int errorLine;
    if (!document.setContent(xmlData, &errorMsg, &errorLine))
    {
        return false;
    }

    QDomElement documentRoot = document.documentElement();
    if (documentRoot.isNull())
    {
        return false;
    }
    if (documentRoot.tagName() != "OpenSCENARIO")
    {
        return false;
    }

    QDomNode vehcileNode = FindNodeByTag(documentRoot.firstChild(), "Vehicle");
    int id = 0;
    while (!vehcileNode.isNull()) {
        if (!ParseParticipants(id, vehcileNode, particpants)){
            return false;
        }
        id++;
        vehcileNode = FindNodeByTag(vehcileNode.nextSibling(), "Vehicle");
    }

    return true;
}

bool VehicleModelImporter::ParseParticipants(int id, QDomNode vehicleNode,
                                             std::map<int, PCM_ParticipantData *> &particpants)
{
    if (vehicleNode.isNull())
    {
        return false;
    }

    // Read vehicle type (OSI domain) and convert it to vehicleType (PCM Domain)
    std::string vehicleString = vehicleNode.toElement().attribute("vehicleCategory").toStdString();
    AgentVehicleType agentVehicleType = GetAgentVehicleType(vehicleString);
    int vehicleTypeId = static_cast<int>(agentVehicleType);
    QString vehicleTypeIdString = QString::number(vehicleTypeId);

    QString mue = "";
    QString weight = "";
    QString ixx = "";
    QString iyy = "";
    QString izz = "";

    QDomNode propertiesNode = FindNodeByTag(vehicleNode, "Properties");
    if (!propertiesNode.isNull())
    {
        QDomNode propertyNode = propertiesNode.firstChild();
        while (!propertyNode.isNull()) {
            QString name = propertyNode.toElement().attribute("name");
            QString value = propertyNode.toElement().attribute("value");

            if( name == "FrictionCoefficient"){
                mue = value;
            }else if (name == "Mass") {
                weight = value;
            }
            else if (name == "MomentInertiaRoll")
            {
                ixx = value;
            }
            else if (name == "MomentInertiaPitch")
            {
                iyy = value;
            }
            else if (name == "MomentInertiaYaw") {
                izz = value;
            }

            propertyNode = propertyNode.nextSibling();
        }
    }

    QString width = "";
    QString length = "";
    QString height = "";
    QString heightcg = "";
    QString cgfront = "";

    QDomNode boundingBoxNode = FindNodeByTag(vehicleNode, "BoundingBox");
    if (!boundingBoxNode.isNull()){
        QDomNode centerNode = FindNodeByTag(boundingBoxNode, "Center");
        QString centerX = "";
        QString centerY = "";
        QString centerZ = "";
        if (!centerNode.isNull()){
            centerX = centerNode.toElement().attribute("x");
            centerY = centerNode.toElement().attribute("y");
            centerZ = centerNode.toElement().attribute("z");

            heightcg = centerZ;
        }

        QDomNode dimensionsNode = FindNodeByTag(boundingBoxNode, "Dimensions");
        if (!dimensionsNode.isNull())
        {
            width = dimensionsNode.toElement().attribute("width");
            length = dimensionsNode.toElement().attribute("length");
            height = dimensionsNode.toElement().attribute("height");
        }

        cgfront = QString::number(length.toDouble() - centerX.toDouble());
    }

    QString wheelbase = "";
    QString trackwidth = "";

    QDomNode axlesNode = FindNodeByTag(vehicleNode, "Axles");
    if (!axlesNode.isNull())
    {
        QDomNode frontAxleNode = FindNodeByTag(axlesNode, "FrontAxle");
        if (!frontAxleNode.isNull())
        {
            trackwidth = frontAxleNode.toElement().attribute("trackWidth");
            wheelbase = frontAxleNode.toElement().attribute("positionX");
        }
    }

    QString distcgfa = cgfront;

    particpants.emplace(std::make_pair(id, new PCM_ParticipantData(vehicleTypeIdString,
                                                                   width,
                                                                   length,
                                                                   distcgfa,
                                                                   weight,
                                                                   heightcg,
                                                                   wheelbase,
                                                                   ixx,
                                                                   iyy,
                                                                   izz,
                                                                   mue,
                                                                   trackwidth,
                                                                   height,
                                                                   cgfront)));

    return true;
}

QDomNode VehicleModelImporter::FindNodeByTag(QDomNode baseNode, std::string tagName)
{
    QDomNode foundNode;

    QDomNode node = baseNode;
    while (!node.isNull()) {
        if (node.toElement().tagName() == QString::fromStdString(tagName)){
            foundNode = node;
        }else{
            foundNode = FindNodeByTag(node.firstChild(), tagName);
        }

        if(foundNode.isNull()){
            node = node.nextSibling();
        }else{
            break;
        }
    }

    return foundNode;
}

