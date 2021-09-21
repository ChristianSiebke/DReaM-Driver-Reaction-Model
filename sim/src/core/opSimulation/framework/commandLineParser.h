/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <list>
#include <string>

#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QCommandLineParser>

#include "common/opExport.h"

struct SIMULATIONCOREEXPORT CommandLineArguments
{
    int logLevel;
    std::string libPath;
    std::string logFile;
    std::string configsPath;
    std::string resultsPath;
};

struct SIMULATIONCOREEXPORT CommandLineOption
{
    QString name;
    QString description;
    QString valueName;
    QString defaultValue;
};

class SIMULATIONCOREEXPORT CommandLineParser
{
public:
    static CommandLineArguments Parse(const QStringList& arguments);
    static std::vector<std::string> GetParsingLog();
private:
    static QList<QCommandLineOption> GetOptions();
    static CommandLineArguments CompileCommandLineArguments(const QCommandLineParser& commandLineParser);
    static void EvaluateDefaultedValues(const QCommandLineParser& commandLineParser);

    static std::vector<std::string> parsingLog;
    static const std::vector<CommandLineOption> commandLineOptions;
};
