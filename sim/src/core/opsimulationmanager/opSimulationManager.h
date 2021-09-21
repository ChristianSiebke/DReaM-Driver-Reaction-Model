/*******************************************************************************
* Copyright (c) 2017, 2019 in-tech GmbH
*               2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \addtogroup opSimulationManager
* @{
*
* \section opSimulationManagerDesc Detailed Description
* This is opSimulationManager module of the framework and it contains the main entry point
* of the framework.
*
* These executeable is started by the GUI with or without an argument to set the
* opSimulationManagerConfig.xml file.
* It reads the opSimulationManagerConfig.xml and saves all options and information.
* Following this a simulation executable is started with the options and information
* as argument.
*
* \section Input arguments
* | arguments | meaning                                                                                                           |
* |-----------|-------------------------------------------------------------------------------------------------------------------|
* | --config  | Path to opSimulationManager configuration file. Working directory and "opSimulationManager.xml" by default.       |
*
* \section Output files
* | file                    | description                                                                                                                            |
* |-------------------------|----------------------------------------------------------------------------------------------------------------------------------------|
* | opSimulationManager.log | Default name for file containing all log messages (depending on the logLevel). Path and name are specified in opSimulationManager.xml. |
*
* @} */
