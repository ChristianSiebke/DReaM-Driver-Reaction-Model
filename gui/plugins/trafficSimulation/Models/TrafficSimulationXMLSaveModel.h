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
//! @file  TrafficSimulationXMLLoadModel.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides routines for saving the slaveConfig XML file.
//!
//!        The saving process and the corrsponding routines strictly follow the structure
//!        of the corresponding combination XML file. In the general structure of such a saving
//!        method, a loop over all xml subelements within the given xml element is incorporated and
//!        the values of the corresponding configuration object are written to the file using the
//!        xml names defined in TrafficSimulationXMLBaseModel. As an xml parser, Qt's QXmlStreamWriter
//!        class is used.
//-----------------------------------------------------------------------------

#ifndef TRAFFICSIMULATIONXMLSAVEMODEL_H
#define TRAFFICSIMULATIONXMLSAVEMODEL_H

#include <QFile>
#include <QXmlStreamWriter>

#include "TrafficSimulationXMLBaseModel.h"
#include "openPASS-TrafficSimulation/TrafficSimulationInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class provides routines for saving the slaveConfig XML file.
//!
//!        The saving process and the corrsponding routines strictly follow the structure
//!        of the corresponding combination XML file. In the general structure of such a saving
//!        method, a loop over all xml subelements within the given xml element is incorporated and
//!        the values of the corresponding configuration object are written to the file using the
//!        xml names defined in TrafficSimulationXMLBaseModel. As an xml parser, Qt's QXmlStreamWriter
//!        class is used.
//-----------------------------------------------------------------------------
class TrafficSimulationXMLSaveModel : public TrafficSimulationXMLBaseModel
{
public:
    explicit TrafficSimulationXMLSaveModel() = delete;
    virtual ~TrafficSimulationXMLSaveModel() = default;

public:
    //-----------------------------------------------------------------------------
    //! This method manages the whole saving process. It opens the given filepath and, on
    //! success, goes through all required configuration objects and calls the corresponding
    //! xml save routines.
    //!
    //! @param[in]      filepath            the filepath of the combination XML file to be written
    //! @param[in]      trafficSimulation   the object containing all information on the simulation configuation
    //!
    //! @return                             "true" if the filepath could be successfully opened, otherwise "false"
    //-----------------------------------------------------------------------------
    static bool save(const QString &filepath,
                     const TrafficSimulationInterface *const trafficSimulation);

    //-----------------------------------------------------------------------------
    //! This method writes all xml subelements of <ExperimentConfig> stored in the
    //! given instance of ExperimentInterface.
    //!
    //! @param[in]      xml                 the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      experiment          the object containing all information on the experiment configuation
    //!
    //-----------------------------------------------------------------------------
    static void saveExperiment(QXmlStreamWriter &xml,
                               ExperimentInterface *experiment);

    //-----------------------------------------------------------------------------
    //! This method writes all Libraries needed by the simulation slave, i.e. all XML elements
    //! of <Libraries> occuring in <ExperimentConfig>.
    //!
    //! @param[in]      xml                 the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      libraries           the pointer to the map of all libraries
    //!
    //-----------------------------------------------------------------------------
    static void saveLibraries(QXmlStreamWriter &xml,
                              ExperimentInterface::Libraries *const libraries);

    //-----------------------------------------------------------------------------
    //! This method writes all logging groups, i.e. all XML elements
    //! of <LoggingGroups> occuring in <ExperimentConfig>.
    //!
    //! @param[in]      xml                 the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      libraries           the pointer to the list of all logging groups
    //!
    //-----------------------------------------------------------------------------
    static void saveLoggingGroups(QXmlStreamWriter &xml,
                                  ExperimentInterface::LoggingGroups *const loggingGroups);

    //-----------------------------------------------------------------------------
    //! This method writes all xml subelements of <EnvironmentConfig> stored in the
    //! given instance of EnvironmentInterface.
    //!
    //! @param[in]      xml                 the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      environment         the object containing all information on the environment configuation
    //!
    //-----------------------------------------------------------------------------
    static void saveEnvironment(QXmlStreamWriter &xml,
                                EnvironmentInterface *environment);

    //-----------------------------------------------------------------------------
    //! This method saves all time of day entries and corresponding probabilities stored
    //! in the given map.
    //!
    //! @param[in]      xml          the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      map          the time of day map, whose values are instances of EnvironmentItemInterface
    //-----------------------------------------------------------------------------
    static void saveTimeOfDayMap(QXmlStreamWriter &xml,
                                 EnvironmentInterface::TimeOfDayMap *map);

    //-----------------------------------------------------------------------------
    //! This method saves all vistibility distance entries and corresponding probabilities stored
    //! in the given map.
    //!
    //! @param[in]      xml          the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      map          the map of visibility distances, whose values are instances of
    //!                              EnvironmentItemInterface
    //-----------------------------------------------------------------------------
    static void saveVisibilityDistanceMap(QXmlStreamWriter &xml,
                                          EnvironmentInterface::VisibilityDistanceMap *map);

    //-----------------------------------------------------------------------------
    //! This method saves all friction coefficients and corresponding probabilities stored
    //! in the given map.
    //!
    //! @param[in]      xml          the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      map          the map of friction coefficinets, whose values are instances of
    //!                              EnvironmentItemInterface
    //-----------------------------------------------------------------------------
    static void saveFrictionMap(QXmlStreamWriter &xml,
                                EnvironmentInterface::FrictionMap *map);

    //-----------------------------------------------------------------------------
    //! This method saves all weather entries and corresponding probabilities stored
    //! in the given map.
    //!
    //! @param[in]      xml          the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      map          the weather map, whose values are instances of
    //!                              EnvironmentItemInterface
    //-----------------------------------------------------------------------------
    static void saveWeatherMap(QXmlStreamWriter &xml,
                               EnvironmentInterface::WeatherMap *map);

    //-----------------------------------------------------------------------------
    //! This method writes all xml subelements of <EgoConfig> and <ScenarioConfig> stored in the
    //! given instance of ScenarioInterface.
    //!
    //! @param[in]      xml                 the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      scenario            the object containing all information on the ego and scenario configuation
    //!
    //-----------------------------------------------------------------------------
    static void saveScenario(QXmlStreamWriter &xml,
                             ScenarioInterface *scenario);

    //-----------------------------------------------------------------------------
    //! This method writes all xml subelements of <TrafficConfig> stored in the
    //! given instance of TrafficInterface. It incorporates the subroutines for
    //! saving traffic parameters and the agents on regular and rightmost lanes.
    //!
    //! @param[in]      xml          the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      traffic      the object containing all information on the traffic configuation
    //-----------------------------------------------------------------------------
    static void saveTraffic(QXmlStreamWriter &xml,
                            TrafficInterface *traffic);

    //-----------------------------------------------------------------------------
    //! This method saves all traffic parameter values and probabilities, i.e. all xml subelements
    //! of <TrafficParameters> stored in the given instance of TrafficInterface.
    //! This comprises traffic volumes, platoon rates, velocities and homogeneities.
    //!
    //! @param[in]      xml          the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      traffic      the object containing all information on the traffic configuation
    //-----------------------------------------------------------------------------
    static void saveTrafficParameters(QXmlStreamWriter &xml,
                                      TrafficInterface *traffic);

    //-----------------------------------------------------------------------------
    //! This method saves all traffic volumes and corresponding probabilities stored
    //! in the given volumes map.
    //!
    //! @param[in]      xml          the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      volumes      the volumes map being an instance of TrafficItemMapInterface with the
    //!                              datatype "int"
    //-----------------------------------------------------------------------------
    static void saveTrafficVolumes(QXmlStreamWriter &xml,
                                   TrafficItemMapInterface *volumes);

    //-----------------------------------------------------------------------------
    //! This method saves all platoon rates and corresponding probabilities stored
    //! in the given platoon rates map.
    //!
    //! @param[in]      xml               the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      platoonRates      the platoon rates map being an instance of TrafficItemMapInterface with the
    //!                                   datatype "double"
    //-----------------------------------------------------------------------------
    static void saveTrafficPlatoonRates(QXmlStreamWriter &xml,
                                        TrafficItemMapInterface *platoonRates);

    //-----------------------------------------------------------------------------
    //! This method saves all velocities and corresponding probabilities stored
    //! in the given velocities map.
    //!
    //! @param[in]      xml               the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      velocities        the velocities map being an instance of TrafficItemMapInterface with the
    //!                                   datatype "double"
    //-----------------------------------------------------------------------------
    static void saveTrafficVelocities(QXmlStreamWriter &xml,
                                      TrafficItemMapInterface *velocities);

    //-----------------------------------------------------------------------------
    //! This method saves all homogeneities and corresponding probabilities stored
    //! in the given homogeneities map.
    //!
    //! @param[in]      xml                  the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      homogeneities        the homogeneities map being an instance of TrafficItemMapInterface with the
    //!                                      datatype "double"
    //-----------------------------------------------------------------------------
    static void saveTrafficHomogeneities(QXmlStreamWriter &xml,
                                         TrafficItemMapInterface *homogeneities);

    //-----------------------------------------------------------------------------
    //! This method saves all regular lane agent profiles and corresponding probabilities stored
    //! in the given map.
    //!
    //! @param[in]      xml           the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      agents        the agents map being an instance of TrafficItemMapInterface with the
    //!                               datatype "QString"
    //-----------------------------------------------------------------------------
    static void saveTrafficRegularLane(QXmlStreamWriter &xml,
                                       TrafficItemMapInterface *agents);

    //-----------------------------------------------------------------------------
    //! This method saves all rightmost lane agent profiles and corresponding probabilities stored
    //! in the given map.
    //!
    //! @param[in]      xml           the instance of QXmlStreamWriter connected to the combination xml file
    //! @param[in]      agents        the agents map being an instance of TrafficItemMapInterface with the
    //!                               datatype "QString"
    //-----------------------------------------------------------------------------
    static void saveTrafficRightmostLane(QXmlStreamWriter &xml,
                                         TrafficItemMapInterface *agents);
};

#endif // TRAFFICSIMULATIONXMLSAVEMODEL_H
