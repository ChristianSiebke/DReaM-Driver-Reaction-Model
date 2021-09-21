/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef CONFIGWRITER_H
#define CONFIGWRITER_H

#include <QDir>

#include "DataStructuresXml/XmlAgentEquipment.h"
#include "DataStructuresXml/XmlModelsConfig.h"
#include "DataStructuresXml/XmlProfilesConfig.h"
#include "DataStructuresXml/XmlScenery.h"
#include "DataStructuresXml/XmlSimulationConfig.h"
#include "DataStructuresXosc/XoscScenario.h"
#include "XmlMergeHelper.h"
#include "pcm_initialValues.h"
#include "pcm_simulationSet.h"

/*!
 * \brief The Config writer class
 * This class is responsible to create and write several configuration files
 */
class ConfigWriter
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor which is given a base folder to start from
    //! @param[in] baseFolder   base Folder from where every config will be generated
    //-----------------------------------------------------------------------------
    ConfigWriter(const QString &baseFolder);

    // removing operators
    ConfigWriter(const ConfigWriter &) = delete;
    ConfigWriter(ConfigWriter &&) = delete;
    ConfigWriter &operator=(const ConfigWriter &) = delete;
    ConfigWriter &operator=(ConfigWriter &&) = delete;

    //-----------------------------------------------------------------------------
    //! Default Destructor
    //-----------------------------------------------------------------------------
    virtual ~ConfigWriter() = default;

    //-----------------------------------------------------------------------------
    //! Generate simulation configuration file.
    //!
    //! @param[in]  configPath          folder name where the config shall be written
    //! @param[in]  simSet              simulation set of data which shall be used
    //! @param[in]  resultFolderName    folder name where the results of the simulation
    //!                                 shall be written
    //! @param[in]  pcmCase             name of pcm case
    //! @param[in]  randomSeed          random seed which shall used in the generation
    //!
    //! @return     whole path and file name of the generated configuration file.
    //-----------------------------------------------------------------------------
    const QString CreateSimulationConfiguration(const QString &configPath,
                                           const PCM_SimulationSet *simSet,
                                           const QString &resultFolderName,
                                           const QString &pcmCase,
                                           const int randomSeed);

    //-----------------------------------------------------------------------------
    //! Generate profiles catalog file.
    //!
    //! @param[in]  configPath          folder name where the config shall be written
    //! @param[in]  simSet              simulation set of data which shall be used
    //! @param[in]  resultFolderName    folder name where the results of the simulation
    //!                                 shall be written
    //! @param[in]  pcmCase             name of pcm case
    //! @param[in]  randomSeed          random seed which shall used in the generation
    //!
    //! @return     whole path and file name of the generated configuration file.
    //-----------------------------------------------------------------------------
    const QString CreateProfilesCatalog(const QString &configPath,
                                        const PCM_SimulationSet *simSet,
                                        const QString &resultFolderName,
                                        const QString &pcmCase,
                                        const int randomSeed);

    //-----------------------------------------------------------------------------
    //! Generate models vehicle file.
    //!
    //! @param[in]  configPath          folder name where the config shall be written
    //! @param[in]  simSet              simulation set of data which shall be used
    //! @param[in]  resultFolderName    folder name where the results of the simulation
    //!                                 shall be written
    //! @param[in]  pcmCase             name of pcm case
    //! @param[in]  randomSeed          random seed which shall used in the generation
    //!
    //! @return     whole path and file name of the generated configuration file.
    //-----------------------------------------------------------------------------
    const QString CreateModelsVehicle(const QString &configPath,
                                      const PCM_SimulationSet *simSet,
                                      const QString &resultFolderName,
                                      const QString &pcmCase,
                                      const int randomSeed);

    //-----------------------------------------------------------------------------
    //! Generate system configuration file.
    //!
    //! @param[in]  configPath         folder name where the config shall be written
    //! @param[in]  otherSystemFile    filename for systems which shall be used for
    //!                                agents other then cars.
    //! @param[in]  car1SystemFile     filename of the system of the first agent
    //! @param[in]  car2SystemFile     filename of the system of the second agent
    //! @param[in]  simSet             simulation set of data which shall be used
    //!
    //! @return     whole path and file name of the generated configuration file.
    //-----------------------------------------------------------------------------
    const QString CreateSystemConfiguration(const QString &configPath,
                                            const QString &otherSystemFile,
                                            const QString &car1SystemFile,
                                            const QString &car2SystemFile,
                                            const PCM_SimulationSet *simSet);

    //-----------------------------------------------------------------------------
    //! Generate scenery configuration file.
    //!
    //! @param[in]  configPath          folder name where the config shall be written
    //! @param[in]  simSet              simulation set of data which shall be used
    //!
    //! @return     whole path and file name of the generated configuration file.
    //-----------------------------------------------------------------------------
    const QString CreateSceneryConfiguration(const QString &configPath,
                                             const PCM_SimulationSet *simSet);

    //-----------------------------------------------------------------------------
    //! Generate parking configuration file.
    //!
    //! @param[in]  configPath          folder name where the config shall be written
    //!
    //! @return     whole path and file name of the generated configuration file.
    //-----------------------------------------------------------------------------
    const QString CreateParkingConfiguration(const QString &configPath);

    //-----------------------------------------------------------------------------
    //! Generate framework configuration file.
    //!
    //! @param[in]  frameworkConfigPath     folder name where the config shall be written
    //! @param[in]  configList              list of generated config files
    //! @param[in]  logLevel                logging level
    //!
    //! @return     whole path and file name of the generated configuration file.
    //-----------------------------------------------------------------------------
    const QString CreateFrameworkConfiguration(const QString frameworkConfigPath,
                                               QList<QMap<QString, QString>> configList,
                                               const int logLevel);

private:
    //-----------------------------------------------------------------------------
    //! Write simulation configuration file.
    //!
    //! @param[in]  simulationConfig     xml config of simulation file
    //! @param[in]  configPath      path where the config file shall be written
    //!
    //! @return     whole path and file name of the written configuration file.
    //-----------------------------------------------------------------------------
    const QString WriteSimulationConfiguration(XmlSimulationConfig &simulationConfig,
                                          const QString &configPath);

    //-----------------------------------------------------------------------------
    //! Write profiles catalog file.
    //!
    //! @param[in]  profilesConfig  xml config of profile catalog file
    //! @param[in]  configPath      path where the config file shall be written
    //!
    //! @return     whole path and file name of the written configuration file.
    //-----------------------------------------------------------------------------
    const QString WriteProfilesCatalog(XmlProfilesConfig &profilesConfig,
                                       const QString &configPath);

    //-----------------------------------------------------------------------------
    //! Write models vehicle catalog file.
    //!
    //! @param[in]  modelsConfig  xml config of models vehicle catalog file
    //! @param[in]  configPath    path where the config file shall be written
    //!
    //! @return     whole path and file name of the written configuration file.
    //-----------------------------------------------------------------------------
    const QString WriteModelsVehicle(XmlModelsConfig &modelsConfig,
                                     const QString &configPath);

    //-----------------------------------------------------------------------------
    //! Write models vehicle catalog file.
    //!
    //! @param[in]  modelsConfig  xml config of models vehicle catalog file
    //! @param[in]  configPath    path where the config file shall be written
    //!
    //! @return     whole path and file name of the written configuration file.
    //-----------------------------------------------------------------------------
    const QString WriteSceneryConfiguration(XmlScenery &sceneryConfig,
                                            const QString &configPath);

    //-----------------------------------------------------------------------------
    //! Write parking configuration file.
    //!
    //! @param[in]  configPath    path where the config file shall be written
    //!
    //! @return     whole path and file name of the written configuration file.
    //-----------------------------------------------------------------------------
    const QString WriteParkingConfiguration(const QString &configPath);

    QDir baseDirectory; //!< base directory from which the configs are generated
};

#endif // CONFIGWRITER_H
