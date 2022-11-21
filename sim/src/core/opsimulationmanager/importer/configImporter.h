/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2017 ITK Engineering GmbH
 *               2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ConfigImporter.h
//! @brief This file contains the importer of opSimulationManager configuration.
//-----------------------------------------------------------------------------

#pragma once

#include <QFile>
#include <QDomElement>
#include <QString>
#include <vector>
#include "../framework/simulationConfig.h"

namespace SimulationManager {
namespace Configuration {

class Config;
using SimulationConfigs = std::vector<SimulationConfig>;

class ConfigImporter
{
public:
    ConfigImporter() = delete;
    ConfigImporter(const ConfigImporter&) = delete;
    ConfigImporter(ConfigImporter&&) = delete;
    ConfigImporter& operator=(const ConfigImporter&) = delete;
    ConfigImporter& operator=(ConfigImporter&&) = delete;
    virtual ~ConfigImporter() = delete;

    //-------------------------------------------------------------------------
    //! \brief Imports opSimulationManager configuration details from opSimulationManager
    //! 	   configuration xml
    //! \param[in] filename The name of opSimulationManager configuration xml file to
    //! 		   parse
    //! \returns A Config object containing the details of opSimulationManager
    //! 		 configuration as detailed in opSimulationManager configuration xml
    //-------------------------------------------------------------------------
    static Config Import(const QString& filename);

private:
    //-------------------------------------------------------------------------
    //! \brief Parses simulation configuration details from the opSimulationManager
    //! configuration xml
    //! \param[in] element The Xml Element containing the simulation configuration
    //! 		   data
    //! \returns A SimulationConfig object containing the details of the simulation
    //! 		 configuration as detailed in the xml element
    //-------------------------------------------------------------------------
    static SimulationConfig ParseSimulationConfig(const QDomElement& element);

    //-------------------------------------------------------------------------
    //! \brief Converts a filename to an absolute file path, if necessary.
    //! 	   Throws if no file exists at the location specified by the
    //! 	   absolute file path.
    //! \param[in] filename The filename to convert to an absolute file path.
    //! \returns A QString representing the absolute filepath to filename.
    //-------------------------------------------------------------------------
    static QString GetAbsoluteFilePath(const QString& filename);

    //-------------------------------------------------------------------------
    //! \brief Prepares an XML file for parsing using QDomElements
    //! \param[in] xmlFile The QFile object to be parsed with QDomElements
    //! \param[in] file The path of the file xmlFile refers to
    //! \returns A QDomElement referring to the root element of the XML file
    //-------------------------------------------------------------------------
    static QDomElement ReadDocument(QFile& xmlFile, const std::string& file);

    //-------------------------------------------------------------------------
    //! \brief Parses multiple simulations' configuration details from the
    //!        opSimulationManager configuration xml
    //! \param[in] element The parent of the xml elements containing the
    //! 		   simulations' configuration data
    //! \returns A std:vector<SimulationConfig> containing the details of the
    //! 		 simulations' configurations as detailed in the xml element
    //-------------------------------------------------------------------------
    static SimulationConfigs ParseSimulationConfigs(const QDomElement& element);
};

} // namespace Configuration
} // namespace SimulationManager
