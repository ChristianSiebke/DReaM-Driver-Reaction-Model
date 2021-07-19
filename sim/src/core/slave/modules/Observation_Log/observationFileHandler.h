/*******************************************************************************
* Copyright (c) 2019, 2020, 2021 in-tech GmbH
*               2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <string>

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTextStream>
#include <QXmlStreamWriter>

#include "include/eventNetworkInterface.h"
#include "include/observationInterface.h"
#include "include/dataBufferInterface.h"
#include "observationCyclics.h"
#include "observationLogConstants.h"
#include "runStatistic.h"

struct Event
{
    Event (const int& time, const openpass::databuffer::AcyclicRow& dataRow) :
        time(time),
        dataRow(dataRow)
    {}

    int time;
    openpass::databuffer::AcyclicRow dataRow;
};

using Events = std::vector<Event>;

//-----------------------------------------------------------------------------
/** \brief Provides the basic logging and observer functionality.
*   \details Provides the basic logging functionality.
*            Writes the framework, events, agents and samples into the output.
*            It has two logging modes either event based or as single output.
*            This module also acts as an observer of agent modules.
*
*   \ingroup ObservationLog
*/
//-----------------------------------------------------------------------------
class ObservationFileHandler
{
public:
    const std::string COMPONENTNAME = "ObservationFileHandler";

    ObservationFileHandler(const DataBufferReadInterface& dataBuffer);
    ObservationFileHandler(const ObservationFileHandler &) = delete;
    ObservationFileHandler(ObservationFileHandler &&) = delete;
    ObservationFileHandler &operator=(const ObservationFileHandler &) = delete;
    ObservationFileHandler &operator=(ObservationFileHandler &&) = delete;
    ~ObservationFileHandler() = default;

    /*!
     * \brief Sets the directory where the output is written into
     *
     * \param outputDir path of output directory
     */
    void SetOutputLocation(const std::string& outputDir, const std::string& outputFile)
    {
        folder = QString::fromStdString(outputDir);
        finalFilename = QString::fromStdString(outputFile);
        finalPath = folder + QDir::separator() + finalFilename;
    }

    void SetSceneryFile(const std::string& fileName)
    {
        sceneryFile = fileName;
    }

    void SetCsvOutput(bool writeCsv)
    {
        writeCyclicsToCsv = writeCsv;
    }

    /*!
     * \brief Creates the output file as simulationOutput.tmp and writes the basic header information
     */
    void WriteStartOfFile(const std::string &frameworkVersion);

    /*!
     * \brief This function gets called after each run and writes all information about this run into the output file
     *
     * \param runResult
     * \param runStatistik
     * \param cyclics
     * \param dataBuffer
     * \param events
     */
    void WriteRun(const RunResultInterface& runResult, RunStatistic runStatistik, ObservationCyclics& cyclics, const Events& events);

    /*!
     * \brief Closes the xml tags flushes the output file, closes it and renames it to simulationOutput.xlm
     */
    void WriteEndOfFile();

protected:
    std::unique_ptr<QXmlStreamWriter> xmlFileStream;
    const DataBufferReadInterface& dataBuffer;

    int runNumber; //!< run number
    std::string sceneryFile;

    bool writeCyclicsToCsv{false};

    OutputAttributes outputAttributes;
    OutputTags outputTags;

    QString folder;
    QString tmpFilename;
    QString finalFilename;
    QString tmpPath;
    QString finalPath;
    std::unique_ptr<QTemporaryFile> xmlFile;

    //add infos to the file stream
    /*!
    * \brief Returns wether the agent has sensors or not.
    *
    * @return       true if agent has sensors, otherwise false.
    */
    inline bool ContainsSensor(const openpass::sensors::Parameters &sensorParameters) const;

    /*!
    * \brief Writes the sensor information into the simulation output.
    *
    * @param[in]    agentId     id of the agent
    * @param[in]    sensorId    id of the sensor
    */
    void AddSensor(const std::string& agentId, const std::string& sensorId);

    /*!
    * \brief Writes the sensor information into the simulation output.
    *
    * @param[in]    agentId     id of the agent
    */
    void AddVehicleAttributes(const std::string& agentId);

    /*!
    * \brief Writes all sensor information of an agent into the simulation output.
    *
    * @param[in]    agentId     id of the agent
    */
    void AddSensors(const std::string& agentId);

    /*!
    * \brief Writes the content of an agent into the simulation output.
    *
    * @param[in]    agentId     id of the agent
    */
    void AddAgent(const std::string& agentId);

    /*!
    * \brief Writes the content of all agent into the simulation output.
    */
    void AddAgents();

    /*!
    * \brief Writes all events into the simulation output.
    *
    * @param[in]     events     events of the run
    */
    void AddEvents(const Events &events);

    /*!
    * \brief Writes the header into the simulation output during full logging.
    *
    * @param[in]     cyclics    cyclics of the run
    */
    void AddHeader(const ObservationCyclics& cyclics);

    /*!
    * \brief Writes the samples into the simulation output during full logging.
    *
    * @param[in]     cyclics    cyclics of the run
    */
    void AddSamples(const ObservationCyclics& cyclics);

    /*!
    * \brief Writes the filename for the cyclics file into the simulation output during full logging.
    *
    * @param[in]     filename           Name of the file, where cyclics are written to.
    */
    void AddReference(QString filename);

    /*!
    * \brief Removes old cyclic files from directory.
    *
    * @param[in]    directory           directory to delete the cyclic files
    */
    void RemoveCsvCyclics(QString directory);

    /*!
    * \brief Writes the cyclics of one run to a csv.
    *
    * @param[in]    filepath            Filepath for current run
    * @param[in]    cyclics             Cyclics of the current run
    */
    void WriteCsvCyclics(const QString &filepath, const ObservationCyclics &cyclics);

    /*!
    * \brief Write entities to XML
    *
    * \param tag       tag name
    * \param entities  list of entities
    * \param mandatory if set, an emtpy tag is added if entities is empty (default false)
    */
    void WriteEntities(const QString tag, const openpass::type::EntityIds &entities, bool mandatory = false);

    /*!
    * \brief Write (event) parameter to XML
    *
    * \remark Might be used for generic parameters in the future - right now, only event parameters
    *
    * \param[in]    parameters  list of parameters
    * \param[in]    mandatory   if set, an emtpy tag is added if parameters are empty (default false)
    */
    void WriteParameter(const openpass::type::FlatParameter &parameters, bool mandatory = false);

    const QString outputFileVersion = "0.3.1";
};
