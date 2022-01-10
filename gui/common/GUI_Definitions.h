/********************************************************************************
 * Copyright (c) 2021 ITK Engineering GmbH
 *               2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  GUI_Definitions.h
//! @brief This file contains Definitions of OpenPASS GUI
//-----------------------------------------------------------------------------

#ifndef GUIDEFINITIONS
#define GUIDEFINITIONS

static constexpr char DIRNAME_CASE_RESULTS[] = "results";

static constexpr char FILENAME_SIMULATION_CONFIG[] = "simulationConfig.xml";
static constexpr char FILENAME_SYSTEM_CONFIG[] = "SystemConfig.xml";
static constexpr char FILENAME_SCENERY_CONFIG[] = "sceneryConfiguration.xml";
static constexpr char FILENAME_PARKING_CONFIG[] = "SceneryConfiguration.xodr";
static constexpr char FILENAME_SCENARIO_CONFIG[] = "Scenario.xosc";
static constexpr char FILENAME_PROFILES_CONFIG[] = "ProfilesCatalog.xml";
static constexpr char FILENAME_MODELS_CONFIG[] = "VehicleModelsCatalog.xosc";
static constexpr char FILENAME_FRAMEWORK_CONFIG[] = "opSimulationManager.xml";

static constexpr char FILENAME_OPENPASSSIMULATION_EXE[] = "opSimulation.exe";
static constexpr char FILENAME_OPSIMULATIONMANAGER_EXE[] = "opSimulationManager.exe";

static constexpr char FILENAME_OPENPASSSIMULATION_LOG[] = "opSimulation.log";
static constexpr char FILENAME_OPENPASSSIMULATION_CONFIGS[] = "configs";
static constexpr char FILENAME_OPSIMULATIONMANAGER_LOG[] = "opSimulationManager.log";

static constexpr char REGEX_CASE_NUMBER[] = "\\d*";
static constexpr char REGEX_CASE_SYSTEM[] = "\\d\\-\\d\\-\\d";

static constexpr char DIR_NO_VARIATION[] = "Default";
static constexpr int VARIATION_COUNT_DEFAULT = 2;
static constexpr int INIT_RANDOM_SEED = -1;
static constexpr int SHIFT_RADIUS_CAR1 = 1.0;
static constexpr int SHIFT_RADIUS_CAR2 = 1.0;
static constexpr int VELOCITY_SCALE_CAR1 = 5.0;
static constexpr int VELOCITY_SCALE_CAR2 = 5.0;

#endif // GUIDEFINITIONS
