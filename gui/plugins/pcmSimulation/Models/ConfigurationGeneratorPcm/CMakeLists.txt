# /*********************************************************************
# * Copyright (c) 2021 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

set(SOURCES
        XmlMergeHelper.cpp
        stochasticsPCM.cpp
        openScenarioWriter.cpp
        DatabaseReaderPcm.cpp
        ConfigWriter.cpp
        ConfigSetReaderPcm.cpp
        ConfigGeneratorPcm.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../../common/pcm/PCM_Importer/scenarioImporterPCM.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../../common/pcm/PCM_Importer/vehicleModelImporter.cpp
   )

set(HEADERS
        XmlMergeHelper.h
        stochasticsPCM.h
        openScenarioWriter.h
        DatabaseReaderPcm.h
        ConfigWriter.h
        ConfigSetReaderPcm.h
        ConfigGeneratorPcm.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../../common/pcm/PCM_Importer/scenarioImporterPCM.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../../common/pcm/PCM_Importer/vehicleModelImporter.h
    )

target_sources(${PROJECT_NAME}_Models PRIVATE ${SOURCES} ${HEADERS})

add_subdirectory(DataStructuresXml)
add_subdirectory(DataStructuresXosc)
