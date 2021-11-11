/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#ifndef CONFIGGENERATOR_H
#define CONFIGGENERATOR_H

#include "ConfigWriter.h"
#include "DatabaseReaderPcm.h"
#include "openScenarioWriter.h"
#include "pcm_simulationSet.h"
#include "stochasticsPCM.h"

/*!
 * \brief The Config Generator class
 * This class is responsible to generate several configuration files which are used for the 
 * simulation.
 */
class ConfigGenerator
{
public:
    //-----------------------------------------------------------------------------
    //! Default Constructor
    //-----------------------------------------------------------------------------
    ConfigGenerator();

    //-----------------------------------------------------------------------------
    //! Constructor which is given a base folder to start from
    //! @param[in] baseFolder   base Folder from where every config will be generated
    //-----------------------------------------------------------------------------
    ConfigGenerator(const QString &baseFolder);

    // removing operators
    ConfigGenerator(const ConfigGenerator &) = delete;
    ConfigGenerator(ConfigGenerator &&) = delete;
    ConfigGenerator &operator=(const ConfigGenerator &) = delete;
    ConfigGenerator &operator=(ConfigGenerator &&) = delete;

    //-----------------------------------------------------------------------------
    //! Default Destructor
    //-----------------------------------------------------------------------------
    virtual ~ConfigGenerator();

    //-----------------------------------------------------------------------------
    //! Generate configuration files.
    //!
    //! @param[in]  pcmCase                 name of pcm case
    //! @param[in]  caseOutputFolderName    folder name of the pcm case, where the output
    //!                                     is generated
    //! @param[in]  otherSystemFile         filename for systems which shall be used for
    //!                                     agents other then cars.
    //! @param[in]  car1SystemFile          filename of the system of the first agent
    //! @param[in]  car2SystemFile          filename of the system of the second agent
    //! @param[in]  randomSeed              random seed which shall used in the generation
    //! @param[in]  simulationSet           simulation set of data which shall be used
    //!
    //! @return     true if generation of configs was successful
    //-----------------------------------------------------------------------------
    bool GenerateConfigs(const QString &pcmCase,
                         const QString &caseOutputFolderName,
                         const QString &otherSystemFile,
                         const QString &car1SystemFile,
                         const QString &car2SystemFile,
                         const int randomSeed,
                         const PCM_SimulationSet *simulationSet);

    //-----------------------------------------------------------------------------
    //! Generate the framework configuration file.
    //!
    //! @param[in]  logLevel    log level which shall be set in the configuration file.
    //-----------------------------------------------------------------------------
    const QString GenerateFrameworkConfig(const int logLevel);

    //-----------------------------------------------------------------------------
    //! Add a config set to a list, which is later used to generate the framework config.
    //!
    //! @param[in]  resultFolderName    name of the resultFolder
    //-----------------------------------------------------------------------------
    void AddConfigSet(QString resultFolderName);

    //-----------------------------------------------------------------------------
    //! Clear the current List of generated configs.
    //-----------------------------------------------------------------------------
    void Clear();

private:
    ConfigWriter *configWriter; //!< config writer which used to write all configs
    QString baseFolder = ".";   //!< name of the baseFolder where all configs are generated in

    QList<QMap<QString, QString>> configSetList; //!< List of all generated configs

    //-----------------------------------------------------------------------------
    //! Check if a directory exists and if thats true clear it. At the end create
    //! this directory freshly.
    //!
    //! @param[in]  myDir   Directory which shall created freshly.
    //-----------------------------------------------------------------------------
    bool CheckDirs(const QString myDir);
};

#endif // CONFIGGENERATOR_H
