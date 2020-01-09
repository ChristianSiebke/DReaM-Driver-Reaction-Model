/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <algorithm>
#include "directories.h"
#include <QDir>

namespace openpass::core {

Directories::Directories(const std::string& applicationDir,
                         const std::string& libraryDir,
                         const std::string& configurationDir,
                         const std::string& outputDir):
    baseDir{Directories::Resolve(applicationDir, ".")},
    configurationDir{Directories::Resolve(applicationDir, configurationDir)},
    libraryDir{Directories::Resolve(applicationDir, libraryDir)},
    outputDir{Directories::Resolve(applicationDir, outputDir)}
{}

const std::string Directories::Resolve(const std::string& applicationPath, const std::string& path)
{
    auto qpath = QString::fromStdString(path);

    return QDir::isRelativePath(qpath) ?
           QDir(QString::fromStdString(applicationPath) + QDir::separator() + qpath).absolutePath().toStdString() :
           QDir(qpath).absolutePath().toStdString();
}

const std::string openpass::core::Directories::Concat(const std::string& path, const std::string& file)
{
    return QDir(QString::fromStdString(path) +
                QDir::separator() +
                QString::fromStdString(file)).absolutePath().toStdString();
}

const std::vector<std::string> Directories::Concat(const std::string& path, const std::vector<std::string>& filenames)
{
    std::vector<std::string> result {};

    std::transform(filenames.cbegin(),
                   filenames.cend(),
                   std::back_inserter(result),
                   [&path] (const auto &extension) -> std::string
    {
        return Directories::Concat(path, extension);
    });

    return result;
}

const std::string Directories::StripFile(const std::string& path)
{
    QFileInfo fileInfo(QString::fromStdString(path));
    return fileInfo.path().toStdString();
}

bool Directories::IsRelative(const std::string& path)
{
    QFileInfo fileInfo(QString::fromStdString(path));
    return fileInfo.isRelative();
}

} // namespace openpass::core
