/********************************************************************************
 * Copyright (c) 2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  observation_LogImplementation */
//-----------------------------------------------------------------------------

#include "observation_entityRepositoryImplementation.h"

#include <variant>
#include <vector>

#include <QDir>
#include <QDirIterator>

#include "common/commonTools.h"
#include "common/openPassUtils.h"
#include "include/parameterInterface.h"

ObservationEntityRepositoryImplementation::ObservationEntityRepositoryImplementation(core::EventNetworkInterface *eventNetwork,
    StochasticsInterface *stochastics,
    WorldInterface *world,
    const ParameterInterface *parameters,
    const CallbackInterface *callbacks,
    DataBufferReadInterface *dataBuffer) :
    ObservationInterface(stochastics,
                         world,
                         parameters,
                         callbacks,
                         dataBuffer),
    dataBuffer(dataBuffer)
{
    directory = QString::fromStdString(parameters->GetRuntimeInformation().directories.output);

    const auto &parametersString = parameters->GetParametersString();
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

void ObservationEntityRepositoryImplementation::OpSimulationPreHook()
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

void ObservationEntityRepositoryImplementation::OpSimulationPreRunHook()
{
}

void ObservationEntityRepositoryImplementation::OpSimulationPostRunHook(const RunResultInterface &runResult)
{
    LOGDEBUG("Getting non-persistent entities");
    const auto non_persistent = GetEntities(NON_PERSISTENT_ENTITIES);

    if (writePersitent && runNumber == 0)
    {
        LOGDEBUG("Getting persistent entities");
        persistentEntities = GetEntities(PERSISTENT_ENTITIES);
    }

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

    const auto header = QString::fromStdString(openpass::utils::vector::to_string(CSV_HEADERS, CSV_DELIMITER.toStdString()));

    LOGDEBUG("Writing non-persistent entities");
    stream << header << CSV_DELIMITER << '\n';
    WriteEntities(non_persistent, stream);

    if (writePersitent)
    {
        if (persistentInSeparateFile)
        {
            if (runNumber == 0)
            {
                LOGDEBUG("Writing persistent entities (once only)");
                QString filename = filenamePrefix + "_Persistent.csv";
                QString path = directory + QDir::separator() + filename;
                QFile persistentFile = {path};
                if (!persistentFile.open(QIODevice::WriteOnly | QIODevice::Text))
                {
                    LOGERROR(COMPONENTNAME + " could not create file: " + persistentFile.fileName().toStdString());
                }
                QTextStream persistentStream(&persistentFile);

                persistentStream << header << CSV_DELIMITER << '\n';
                WriteEntities(persistentEntities, persistentStream);
                persistentFile.flush();
                persistentFile.close();
            }
        }
        else
        {
            LOGDEBUG("Writing persistent entities");
            WriteEntities(persistentEntities, stream);
        }
    }

    csvFile.flush();
    csvFile.close();

    ++runNumber;
}

void ObservationEntityRepositoryImplementation::OpSimulationPostHook()
{
}

std::string ObservationEntityRepositoryImplementation::GetEntitySource(const std::string &source_key)
{
    try
    {
        return std::get<std::string>(dataBuffer->GetStatic(source_key).at(0));
    }
    catch (const std::exception &e)
    {
        LOGERROR("While parsing databuffer key " + source_key + ": " + e.what());
        return "<error>";
    }
}

Metainfo ObservationEntityRepositoryImplementation::GetEntityMetainfo(const std::string &metainfo_key)
{
    Metainfo metainfo;
    try
    {
        const auto metainfoKeys = dataBuffer->GetKeys("Statics/" + metainfo_key);
        for (const auto &subkey : metainfoKeys)
        {
            auto value = dataBuffer->GetStatic(metainfo_key + "/" + subkey);
            metainfo.emplace(subkey, to_string(value));
        }
    }
    catch (const std::exception &e)
    {
        LOGERROR("While parsing databuffer key " + metainfo_key + ": " + e.what());
    }
    return metainfo;
}

std::vector<CsvRow> ObservationEntityRepositoryImplementation::GetEntities(const std::string &entityBaseKey)
{
    const auto entityIdStrings = dataBuffer->GetKeys("Statics/" + entityBaseKey);

    // entries in databuffer are not sorted
    std::vector<uint64_t> entityIds;
    entityIds.reserve(entityIdStrings.size());
    std::transform(entityIdStrings.cbegin(), entityIdStrings.cend(),
                   std::back_inserter(entityIds),
                   [](const std::string &s) { return stoi(s); });
    std::sort(entityIds.begin(), entityIds.end());

    std::vector<CsvRow> csvRows;

    for (const auto &entityId : entityIds)
    {
        const auto entityKey = entityBaseKey + "/" + std::to_string(entityId);
        csvRows.emplace_back(
            entityId,
            GetEntitySource(entityKey + "/Source"),
            GetEntityMetainfo(entityKey + "/Metainfo"));
    }

    return csvRows;
}

void ObservationEntityRepositoryImplementation::WriteEntities(const std::vector<CsvRow> &entities, QTextStream &stream)
{
    for (const auto &row : entities)
    {
        stream << QString::number(row.id) << CSV_DELIMITER
               << QString::fromStdString(row.source) << CSV_DELIMITER;

        for (const auto &columnMapping : CSV_METAINFO)
        {
            for (const auto &column : columnMapping)
            {
                if (const auto &iter = row.metainfo.find(column); iter != row.metainfo.cend())
                {
                    stream << QString::fromStdString(iter->second);
                }
                stream << CSV_DELIMITER;
            }
        }
        stream << '\n';
    }
}

const std::string ObservationEntityRepositoryImplementation::to_string(const openpass::databuffer::Values &values)
{
    if (values.empty())
    {
        LOGERROR("Retrieved value from databuffer contains no element");
        return "<error>";
    }

    if (values.size() > 1)
    {
        LOGWARN("Multiple values per databuffer request currently not supported");
    }

    return std::visit(openpass::utils::FlatParameter::to_string(), values.front());
}
