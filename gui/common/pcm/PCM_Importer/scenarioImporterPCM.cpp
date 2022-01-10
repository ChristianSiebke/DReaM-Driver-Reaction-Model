/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "scenarioImporterPCM.h"

bool ScenarioImporterPCM::Import(const std::string &filename,
                                 std::map<int, PCM_Trajectory *> &trajectories)
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

    QDomNode storyBoardNode = documentRoot.firstChild();

    QDomNode storyNode = FindNodeByTag(storyBoardNode, "Story");
    while (!storyNode.isNull()) {
        QDomNode actNode = FindNodeByTag(storyNode, "Act");
        int id = 0;
        while(!actNode.isNull()){
            QDomNode trajectoryNode = FindNodeByTag(actNode, "Trajectory");
            if (!trajectoryNode.isNull()) {
                if (!ParseTrajectories(id, trajectoryNode, trajectories)){
                    return false;
                }
            }else{
                return false;
            }
            id++;
            actNode = FindNodeByTag(actNode.nextSibling(), "Act");
        }

        storyNode = FindNodeByTag(storyNode.nextSibling(), "Story");
    }

    QDomNode initNode = FindNodeByTag(storyBoardNode, "Init");
    if (!ParseAbsoluteTargetSpeed(initNode, trajectories))
    {
        return false;
    }

    return true;
}

bool ScenarioImporterPCM::ParseAbsoluteTargetSpeed(QDomNode initNode, std::map<int, PCM_Trajectory *> &trajectories)
{
    if (!initNode.isNull())
    {
        QDomNode privateNode = FindNodeByTag(initNode, "Private");
        int id = 0;
        while (!privateNode.isNull())
        {
            QDomNode absoluteTargetSpeedNode = FindNodeByTag(privateNode, "AbsoluteTargetSpeed");
            if (!absoluteTargetSpeedNode.isNull())
            {
                std::vector<double> *velVec = trajectories.at(id)->GetUVelVec();
                double absoluteTargetSpeed = absoluteTargetSpeedNode.toElement().attribute("value").toDouble();

                if (velVec != nullptr)
                {
                    velVec->emplace(velVec->begin(), absoluteTargetSpeed);
                }
            }
            else
            {
                return false;
            }

            privateNode = FindNodeByTag(privateNode.nextSibling(), "Private");
            id++;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool ScenarioImporterPCM::ParseTrajectories(int id,
                                            QDomNode trajectoriesNode,
                                            std::map<int, PCM_Trajectory *> &trajectories)
{
    if (trajectoriesNode.isNull())
    {
        return false;
    }

    std::vector<double> *timeVec = new std::vector<double>();
    std::vector<double> *xPosVec = new std::vector<double>();
    std::vector<double> *yPosVec = new std::vector<double>();
    std::vector<double> *xVelVec = new std::vector<double>();
    std::vector<double> *yVelVec = new std::vector<double>();
    std::vector<double> *angleVec = new std::vector<double>();

    QDomNode vertexNode = FindNodeByTag(trajectoriesNode, "Vertex");
    while (!vertexNode.isNull())
    {
        double time = vertexNode.toElement().attribute("time").toDouble();
        timeVec->push_back(time);

        QDomNode positionNode = vertexNode.firstChild();
        QDomNode worldNode = positionNode.firstChild();

        double x = worldNode.toElement().attribute("x").toDouble();
        xPosVec->push_back(x);
        double y = worldNode.toElement().attribute("y").toDouble();
        yPosVec->push_back(y);
        double h = worldNode.toElement().attribute("h").toDouble();
        angleVec->push_back(h);

        if(timeVec->size() > 1){
            // approximation for velocity
            double vel = sqrt(pow((xPosVec->end()[-2] - xPosVec->end()[-1]), 2) + pow((yPosVec->end()[-2] - yPosVec->end()[-1]), 2)) / abs(timeVec->end()[-2] - timeVec->end()[-1]);

            xVelVec->push_back(vel);
            yVelVec->push_back(0);
        }

        vertexNode = vertexNode.nextSibling();
    }
    // copy last velocity value to fill velocity vector
    double lastVel = xVelVec->back();

    xVelVec->push_back(lastVel);
    yVelVec->push_back(0);

    trajectories.emplace(std::make_pair(id, new PCM_Trajectory(timeVec,
                                                               xPosVec,
                                                               yPosVec,
                                                               xVelVec,
                                                               yVelVec,
                                                               angleVec)));

    return true;
}

QDomNode ScenarioImporterPCM::FindNodeByTag(QDomNode baseNode, std::string tagName)
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

