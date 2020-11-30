/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  observation_LogImplementation */
//-----------------------------------------------------------------------------

#include <unordered_map>
#include <variant>
#include <vector>
#include <QDir>
#include <QDirIterator>

#include "common/commonTools.h"
#include "common/openPassUtils.h"
#include "include/parameterInterface.h"

#include "observation_entityRepositoryImplementation.h"

constexpr openpass::type::EntityId firstPersistentId = 100000;
const QString DELIMITER {";"};

ObservationEntityRepositoryImplementation::ObservationEntityRepositoryImplementation(
        SimulationSlave::EventNetworkInterface* eventNetwork,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        const ParameterInterface* parameters,
        const CallbackInterface* callbacks,
        DataStoreReadInterface* dataStore) :
    ObservationInterface(stochastics,
                         world,
                         parameters,
                         callbacks,
                         dataStore),
    dataStore(dataStore)
{
    directory = QString::fromStdString(parameters->GetRuntimeInformation().directories.output);

    const auto& parametersString = parameters->GetParametersString();
    const auto filenamePrefixParameter = parametersString.find("FilenamePrefix");
    if (filenamePrefixParameter != parametersString.cend())
    {
        filenamePrefix = QString::fromStdString(filenamePrefixParameter->second);
    }

    const auto persistentParameter = parametersString.find("WritePersistentEntities");
    if (persistentParameter != parametersString.cend())
    {
        if (persistentParameter->second == "Consolidated")
        {
            writePersitent = true;
            persistentInSeparateFile = false;
        }
        else if (persistentParameter->second == "Separate")
        {
            writePersitent = true;
            persistentInSeparateFile = true;
        }
        else if (persistentParameter->second == "Skip")
        {
            writePersitent = false;
        }
        else
        {
            LOGERRORANDTHROW("Unknown parameter \"" + persistentParameter->second + "\"")
        }
    }
}

void ObservationEntityRepositoryImplementation::SlavePreHook()
{
    QDirIterator it(directory, QStringList() << filenamePrefix + "_*.csv", QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        if (fileInfo.baseName().startsWith(filenamePrefix) && fileInfo.suffix() == "csv")
        {
            QFile::remove(fileInfo.filePath());
        }
    }
}

void ObservationEntityRepositoryImplementation::SlavePreRunHook()
{
}

void ObservationEntityRepositoryImplementation::SlavePostRunHook(const RunResultInterface& runResult)
{
    using namespace std::string_literals;
    QString runPrefix = "";
    if (runNumber < 10)
    {
        runPrefix = "00";
    }
    else if (runNumber < 100)
    {
        runPrefix = "0";
    }

    QString filename = filenamePrefix + "_Run_" + runPrefix + QString::number(runNumber) + ".csv";

    QString path = directory + QDir::separator() + filename;
    QFile csvFile = {path};
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        LOGERRORANDTHROW(COMPONENTNAME + " could not create file: " + csvFile.fileName().toStdString());
    }

    QTextStream stream(&csvFile);

    const std::vector<std::string> columnHeaders {"time"s, "id"s, "source"s, "version"s, "secondary id", "type"s, "subtype"s, "name"};
    stream << QString::fromStdString(openpass::utils::vector::to_string(columnHeaders, DELIMITER.toStdString())) << '\n';

    const auto entities = dataStore->GetAcyclic(std::nullopt, std::nullopt, "Entities");
    AcyclicRowRefs agents;
    std::copy_if(entities->begin(), entities->end(), std::back_inserter(agents),
                 [&](const AcyclicRow& row){return row.entityId < firstPersistentId;});
    LOGDEBUG("Writing agents");
    WriteEntities(agents, stream);

    if (writePersitent)
    {
        if (persistentInSeparateFile)
        {
            if (runNumber == 0)
            {
                QString filename = filenamePrefix + "_Persistent.csv";
                QString path = directory + QDir::separator() + filename;
                QFile persistentFile = {path};
                if (!persistentFile.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    LOGERROR(COMPONENTNAME + " could not create file: " + persistentFile.fileName().toStdString());
                }
                QTextStream persistentStream(&persistentFile);

                persistentStream << QString::fromStdString(openpass::utils::vector::to_string(columnHeaders, DELIMITER.toStdString())) << '\n';

                AcyclicRowRefs persistentEntities;
                std::copy_if(entities->begin(), entities->end(), std::back_inserter(persistentEntities),
                             [&](const AcyclicRow& row){return row.entityId >= firstPersistentId;});
                LOGDEBUG("Writing persistent entities into seperate file");
                WriteEntities(persistentEntities, persistentStream);
                persistentFile.flush();
                persistentFile.close();
            }
        }
        else
        {
            AcyclicRowRefs persistentEntities;
            std::copy_if(entities->begin(), entities->end(), std::back_inserter(persistentEntities),
                         [&](const AcyclicRow& row){return row.entityId >= firstPersistentId;});
            LOGDEBUG("Writing persistent entities");
            WriteEntities(persistentEntities, stream);
        }
    }

    csvFile.flush();
    csvFile.close();

    ++runNumber;
}

void ObservationEntityRepositoryImplementation::SlavePostHook()
{
}

void ObservationEntityRepositoryImplementation::WriteEntities(AcyclicRowRefs& entities, QTextStream& stream)
{
    using namespace std::string_literals;

    for (const AcyclicRow& entity : entities)
    {
        std::string debugMessage{"Writing entity " + std::to_string(entity.entityId) + " with the following parameters: "};
        for (const auto& [key, value] : entity.data.parameter)
        {
            debugMessage += key + ",";
        }
        LOGDEBUG(debugMessage);
        stream << QString::number(entity.timestamp.value) << DELIMITER
               << QString::number(entity.entityId.value) << DELIMITER
               << QString::fromStdString(entity.data.name) << DELIMITER;

        const std::vector<std::string> metaInfo {"version"s, "id", "type"s, "subtype"s, "name"s};
        for(const auto& entry : metaInfo)
        {
            // No structured binding on purpose:
            // see https://stackoverflow.com/questions/46114214/lambda-implicit-capture-fails-with-variable-declared-from-structured-binding
            for (const auto &p : entity.data.parameter)
            {
                auto parameterWriter = [&](const std::string &value) {

                    if (p.first == entry)
                    {
                        stream << QString::fromStdString(value);
                    }
                };

                std::visit(openpass::utils::FlatParameter::to_string(parameterWriter), p.second);
            }

            stream << (entry != metaInfo.back() ? DELIMITER : QString("\n"));
        }
    }
}
