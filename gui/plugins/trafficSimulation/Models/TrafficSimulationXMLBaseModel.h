/******************************************************************************
* Copyright (c) 2019 Volkswagen Group of America.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  TrafficSimulationXMLBaseModel.h
//! @ingroup trafficSimulationPlugin
//! @brief This class defines the XML element names of the combination XML file.
//!
//!        Further, it contains lists of subelements if an XML element has subelements.
//!        These lists are used to check that all required subelements are contained when
//!        loading a combination XML file.
//! @note  Since this class contains a variety QStrings with the XML element names, we refrain
//!        from a detailed documentation of the class attributes. Looking at the .cpp files, it
//!        should be immediately clear what the variables contains.
//-----------------------------------------------------------------------------

#ifndef TRAFFICSIMULATIONXMLBASEMODEL_H
#define TRAFFICSIMULATIONXMLBASEMODEL_H

#include <QList>
#include <QString>

//-----------------------------------------------------------------------------
//! @brief This class defines the XML element names of the combination XML file.
//!
//!        Further, it contains lists of subelements if an XML element has subelements.
//!        These lists are used to check that all required subelements are contained when
//!        loading a combination XML file.
//!
//! @note  Since this class contains a variety QStrings with the XML element names, we refrain
//!        from a detailed documentation of the class attributes. Looking at the .cpp files, it
//!        should be immediately clear what the variables contains.
//-----------------------------------------------------------------------------
class TrafficSimulationXMLBaseModel
{
public:
    explicit TrafficSimulationXMLBaseModel() = delete;
    virtual ~TrafficSimulationXMLBaseModel() = default;

protected:
    static QString const KeyRoot;
    static QString const KeyProfilesCatalogue;

protected:
    static QString const KeyExperiment;
    static QString const KeyExperimentID;
    static QString const KeyExperimentNumberInvocations;
    static QString const KeyExperimentSeed;
    static QString const KeyExperimentLibraries;
    static QString const KeyExperimentLoggingGroups;
    static QString const KeyExperimentLoggingGroup;
    static QList<QString> const KeyListExperiment;

protected:
    static QString const KeyEnvironment;
    static QString const KeyTimeOfDayMap;
    static QString const KeyVisibilityDistanceMap;
    static QString const KeyFrictionMap;
    static QString const KeyWeatherMap;
    static QList<QString> const KeyListEnvironment;

protected:
    static QString const KeyTimeOfDay;
    static QString const KeyVisibilityDistance;
    static QString const KeyFriction;
    static QString const KeyWeather;

protected:
    static QString const KeyEgo;
    static QString const KeyEgoAgent;
    static QList<QString> const KeyListEgo;

protected:
    static QString const KeyScenario;
    static QString const KeyScenarioFile;
    static QString const KeyScenarioAgents;
    static QString const KeyScenarioAgent;
    static QList<QString> const KeyListScenario;

protected:
    static QString const KeyTraffic;
    static QString const KeyTrafficParameters;
    static QString const KeyTrafficRegularLane;
    static QString const KeyTrafficRightmostLane;

    static QList<QString> const KeyListTraffic;
    static QList<QString> const KeyListTrafficParameters;

    static QString const KeyTrafficAgent;
    static QString const KeyTrafficVolumes;
    static QString const KeyTrafficVolume;
    static QString const KeyTrafficPlatoonRates;
    static QString const KeyTrafficPlatoonRate;
    static QString const KeyTrafficVelocities;
    static QString const KeyTrafficVelocity;
    static QString const KeyTrafficHomogeneities;
    static QString const KeyTrafficHomogeneity;
};

#endif // TRAFFICSIMULATIONXMLBASEMODEL_H
