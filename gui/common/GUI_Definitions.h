/******************************************************************************
* Copyright (c) 2017 ITK Engineering GmbH.
* Copyright (c) 2018 in-tech GmbH.
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  GUI_Definitions.h
//! @brief This file contains Definitions of OpenPASS GUI
//-----------------------------------------------------------------------------

#ifndef GUIDEFINITIONS
#define GUIDEFINITIONS

#define DIRNAME_CASE_RESULTS        "results"

#define FILENAME_RUN_CONIG          "slaveConfig.xml"
#define FILENAME_SYSTEM_CONIG       "SystemConfig.xml"
#define FILENAME_SCENERY_CONIG      "sceneryConfiguration.xml"
#define FILENAME_PARKING_CONIG      "SceneryConfiguration.xodr"
#define FILENAME_SCENARIO_CONIG     "Scenario.xosc"
#define FILENAME_PROFILES_CONIG     "ProfilesCatalog.xml"
#define FILENAME_MODELS_CONIG       "VehicleModelsCatalog.xosc"
#define FILENAME_FRAMEWORK_CONIG    "masterConfig.xml"

#define FILENAME_OPENPASSSLAVE_EXE  "OpenPassSlave.exe"
#define FILENAME_OPENPASSMASTER_EXE "OpenPassMaster.exe"

#define FILENAME_OPENPASSSLAVE_LOG  "OpenPassSlave.log"
#define FILENAME_OPENPASSSLAVE_CONFIGS  "configs"
#define FILENAME_OPENPASSMASTER_LOG  "OpenPassMaster.log"

#define REGEX_CASE_NUMBER           "\\d*"
#define REGEX_CASE_SYSTEM           "\\d\\-\\d\\-\\d"

#define DIR_NO_VARIATION            "Default"
#define VARIATION_COUNT_DEFAULT     2
#define INIT_RANDOM_SEED            -1
#define SHIFT_RADIUS_CAR1           1.0
#define SHIFT_RADIUS_CAR2           1.0
#define VELOCITY_SCALE_CAR1         5.0
#define VELOCITY_SCALE_CAR2         5.0

#endif // GUIDEFINITIONS
