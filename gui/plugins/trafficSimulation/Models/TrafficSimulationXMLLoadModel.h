/********************************************************************************
 * Copyright (c) 2019 Volkswagen Group of America
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  TrafficSimulationXMLLoadModel.h
//! @ingroup trafficSimulationPlugin
//! @brief This class provides routines for loading the combination XML file and
//!        for storing all information in the corresponding instances of the model
//!        classes.
//!
//!        The loading and the corrsponding routines strictly follow the structure
//!        of the corresponding combination XML file. In the general structure of such a loading
//!        method, a loop over all xml subelements within the given xml element is incorporated
//!        During this loop, the loaded subelement names are compared to the keywords defined
//!        in TrafficSimulationXMLBaseModel. In case of a match, the corresponding element text
//!        is read and the information is stored in the proper attribute of the configuration object.
//!        Unknown xml elements are generally skipped. Further, it is checked whether all required elements
//!        are listed in the file. If not, the routine returns "false", otherwise "true".  In order
//!        to check for xml rules, the QXmlStreamReader class is used for parsing.
//-----------------------------------------------------------------------------
#ifndef TRAFFICSIMULATIONXMLLOADMODEL_H
#define TRAFFICSIMULATIONXMLLOADMODEL_H

#include <QFile>
#include <QXmlStreamReader>

#include "Models/TrafficSimulationXMLBaseModel.h"
#include "openPASS-TrafficSimulation/TrafficSimulationInterface.h"

//-----------------------------------------------------------------------------
//! @brief This class provides routines for loading the combination XML file and
//!        for storing all information in the corresponding instances of the model
//!        classes.
//!
//!        The loading and the corrsponding routines strictly follow the structure
//!        of the corresponding combination XML file. In the general structure of such a loading
//!        method, a loop over all xml subelements within the given xml element is incorporated
//!        During this loop, the loaded subelement names are compared to the keywords defined
//!        in TrafficSimulationXMLBaseModel. In case of a match, the corresponding element text
//!        is read and the information is stored in the proper attribute of the configuration object.
//!        Unknown xml elements are generally skipped. Further, it is checked whether all required elements
//!        are listed in the file. If not, the routine returns "false", otherwise "true". In order
//!        to check for xml rules, the QXmlStreamReader class is used for parsing.
//-----------------------------------------------------------------------------
class TrafficSimulationXMLLoadModel : public TrafficSimulationXMLBaseModel
{
public:
    explicit TrafficSimulationXMLLoadModel() = delete;
    virtual ~TrafficSimulationXMLLoadModel() = default;

public:
    //-----------------------------------------------------------------------------
    //! This method manages the whole loading process and calls the subroutines for
    //! loading the particular sections of the simulationConfig XML file. This routine opens
    //! the submitted file and connects it to an QXmlStreamReader instance.
    //!
    //! @param[in]       filepath           the filepath of the combination xml file
    //! @param[in]       trafficSimulation  pointer to the traffic simulation interface for
    //!                                     storing the loaded configuration
    //! @param[in]       agents             pointer to the agent map of the agent
    //!                                     configuration plugin
    //!
    //! @return                             "true" if given filepath can be opened and all
    //!                                     loading subroutines finished successfully. "false"
    //!                                     otherwise
    //-----------------------------------------------------------------------------
    static bool load(const QString &filepath,
                     TrafficSimulationInterface *const trafficSimulation);

    static bool loadAgentProfiles(TrafficSimulationInterface::ProfilesCatalogue catalogue,
                                  QList<QString> &agents);

    //-----------------------------------------------------------------------------
    //! This method reads all xml elements connected to <ExperimentConfig> from the simulationConfig
    //! xml file and stores the corresponding values in in the given ExperimentInterface instance.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       experiment     the ExperimentInterface instance in which values are stored
    //!
    //! @return                         "true", if all necessary xml elements have been read, otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    static bool loadExperiment(QXmlStreamReader &xml,
                               ExperimentInterface *experiment);

    //-----------------------------------------------------------------------------
    //! This method reads all xml elements of <Libraries> within the <ExperimentConfig> element.
    //! These correspond to the libraries needed by the simulation.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       libraries      the pointer to the libraries map
    //!
    //! @return                         so far always "true"
    //-----------------------------------------------------------------------------
    static bool loadLibraries(QXmlStreamReader &xml,
                              ExperimentInterface::Libraries *const libraries);

    //-----------------------------------------------------------------------------
    //! This method reads all xml elements of <LoggingGroups> within the <ExperimentConfig> element.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       libraries      the pointer to the list of logging groups
    //!
    //! @return                         "true", if all read xml elements are named "LoggingGroup",
    //!                                 "false", othewise.
    //-----------------------------------------------------------------------------
    static bool loadLoggingGroups(QXmlStreamReader &xml,
                                  ExperimentInterface::LoggingGroups *const loggingGroups);

    //-----------------------------------------------------------------------------
    //! This method reads all xml elements connected to <EnvironmentConfig> from the combination
    //! xml file and stores the corresponding values in in the given EnvironmentInterface instance.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       experiment     the EnvironmentInterface instance in which values are stored
    //!
    //! @return                         "true", if all necessary xml elements have been read, otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    static bool loadEnvironment(QXmlStreamReader &xml,
                                EnvironmentInterface *environment);

    //-----------------------------------------------------------------------------
    //! This method reads all time of day entries from the
    //! combination xml file and stores their values and probabilities.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       environment    the pointer to the environment configuration, i.e. the instance
    //!                                 of EnvironmentInterface
    //!
    //! @return                         "true", if all listed xml elements are of type "TimeOfDay", otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    static bool loadTimeOfDayMap(QXmlStreamReader &xml,
                                 EnvironmentInterface *environment);

    //-----------------------------------------------------------------------------
    //! This method reads all visibility distances from the
    //! combination xml file and stores their values and probabilities..
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       environment    the pointer to the environment configuration, i.e. the instance
    //!                                 of EnvironmentInterface
    //!
    //! @return                         "true", if all listed xml elements are of type "VisibilityDistance",
    //!                                 otherwise "false"
    //-----------------------------------------------------------------------------
    static bool loadVisibilityDistanceMap(QXmlStreamReader &xml,
                                          EnvironmentInterface *environment);

    //-----------------------------------------------------------------------------
    //! This method reads all friction coefficinets from the
    //! combination xml file and stores their values and probabilities.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       environment    the pointer to the environment configuration, i.e. the instance
    //!                                 of EnvironmentInterface
    //!
    //! @return                         "true", if all listed xml elements are of type "Friction",
    //!                                 otherwise "false"
    //-----------------------------------------------------------------------------
    static bool loadFrictionMap(QXmlStreamReader &xml,
                                EnvironmentInterface *environment);

    //-----------------------------------------------------------------------------
    //! This method reads all weather entries from the
    //! combination xml file and stores their values and probabilities.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       environment    the pointer to the environment configuration, i.e. the instance
    //!                                 of EnvironmentInterface
    //!
    //! @return                         "true", if all listed xml elements are of type "Weather",
    //!                                 otherwise "false"
    //-----------------------------------------------------------------------------
    static bool loadWeatherMap(QXmlStreamReader &xml,
                               EnvironmentInterface *environment);
    //-----------------------------------------------------------------------------
    //! This method reads all xml elements connected to <ScenarioConfig> from the combination
    //! xml file. This comprises the scenario name, the open scenario file and all scenario agents.
    //! For the latter, the subroutine #loadScenarioAgents is used. The corresponding values are
    //! stored in the given instance of ScenarioInterface.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       scenario       the ScenarioInterface instance in which values are stored
    //!
    //! @return                         "true", if all necessary xml elements have been read and
    //!                                 if the loading routine for scenario agents was successful.
    //!                                 Otherwise "false"
    //-----------------------------------------------------------------------------
    static bool loadScenarioConfig(QXmlStreamReader &xml,
                                   ScenarioInterface *scenario);

    static bool loadEntities(ScenarioInterface *scenario, const QString &scenarioFile);

    static bool loadScenarioObjects(QXmlStreamReader &xml,
                                    ScenarioInterface::Entities *entities);

    //-----------------------------------------------------------------------------
    //! This method reads all xml elements connected to <TrafficConfig> from the combination xml file
    //! and stores their values in the given TrafficInterface instance. This comprises all traffic
    //! parameters and the agents on the regular and rightmost lanes.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       traffic        the TrafficInterface instance in which value are stored
    //! @param[in]       configAgents   the agents map of the Agent Configuration Plugin
    //!
    //! @return                         "true", if all necessary traffic elements have been loaded
    //!                                 with success, otherwise "false"
    //-----------------------------------------------------------------------------
    static bool loadTraffic(QXmlStreamReader &xml,
                            TrafficInterface *traffic);

    //-----------------------------------------------------------------------------
    //! This method reads all xml elements connected to <TrafficParameters> from the combination xml file
    //! and stores their values in the given TrafficInterface instance. This comprises the traffic volumes,
    //! platoon rates, velocities and homogeneities, each loaded in a separate subroutine.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       traffic        the TrafficInterface instance in which value are stored
    //!
    //! @return                         "true", if all necessary traffic parameters are covered by
    //!                                 the combination xml and the loading subroutines finished with
    //!                                 success. Otherwise "false"
    //-----------------------------------------------------------------------------
    static bool loadTrafficParameters(QXmlStreamReader &xml,
                                      TrafficInterface *traffic);

    //-----------------------------------------------------------------------------
    //! This method reads all traffic volumes, being subelements of <TrafficParameters>, from the
    //! combination xml file and stores their values and probabilities in the given map.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       volumes        the volumes map, i.e. an instance of TrafficItemMapInterface
    //!                                 with the datatype "int"
    //!
    //! @return                         "true", if all listed xml elements are of type "Volume", otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    static bool loadTrafficVolumes(QXmlStreamReader &xml,
                                   TrafficItemMapInterface *volumes);

    //-----------------------------------------------------------------------------
    //! This method reads all platoon rates, being subelements of <TrafficParameters>, from the
    //! combination xml file and stores their values and probabilities in the given map.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       platoonRates   the platoon rates map, i.e. an instance of TrafficItemMapInterface
    //!                                 with the datatype "double"
    //!
    //! @return                         "true", if all listed xml elements are of type "PlatoonRate", otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    static bool loadTrafficPlatoonRates(QXmlStreamReader &xml,
                                        TrafficItemMapInterface *platoonRates);

    //-----------------------------------------------------------------------------
    //! This method reads all velocities, being subelements of <TrafficParameters>, from the
    //! combination xml file and stores their values and probabilities in the given map.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       velocities     the velocities map, i.e. an instance of TrafficItemMapInterface
    //!                                 with the datatype "double"
    //!
    //! @return                         "true", if all listed xml elements are of type "Velocity", otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    static bool loadTrafficVelocities(QXmlStreamReader &xml,
                                      TrafficItemMapInterface *velocities);

    //-----------------------------------------------------------------------------
    //! This method reads all homogeneities, being subelements of <TrafficParameters>, from the
    //! combination xml file and stores their values and probabilities in the given map.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       homogenieites  the homogeneities map, i.e. an instance of TrafficItemMapInterface
    //!                                 with the datatype "double"
    //!
    //! @return                         "true", if all listed xml elements are of type "Homogeneity", otherwise
    //!                                 "false"
    //-----------------------------------------------------------------------------
    static bool loadTrafficHomogeneities(QXmlStreamReader &xml,
                                         TrafficItemMapInterface *homogeneities);

    //-----------------------------------------------------------------------------
    //! This method reads all agent profiles from the combination xml file and stores their
    //! names and probabilities in the given map.
    //!
    //! @param[in]       xml            the xml stream reader object connected to the combination
    //!                                 xml file
    //! @param[in]       agents         the agents map in which profiles are stored. An instance of
    //!                                 TrafficItemMapInterface with the datatype "QString"
    //!
    //! @return                         "true", if all listed xml elements are of type "AgentProfile" and if
    //!                                 agent profile exists in the agents map of the Agent Configuration Plugin.
    //!                                 Otherwise "false"
    //-----------------------------------------------------------------------------
    static bool loadTrafficAgents(QXmlStreamReader &xml,
                                  TrafficItemMapInterface *agents);
};

#endif // TRAFFICSIMULATIONXMLLOADMODEL_H
