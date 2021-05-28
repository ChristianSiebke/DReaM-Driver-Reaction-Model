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
        ModelPcm_Eval.cpp
        ResultDirItemModel.cpp
        TrajectoryTableModel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Importer/SceneryImporterPCM.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_agent.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_course.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_globalData.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_line.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_lineContainer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_marks.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_object.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_point.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_pointContainer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_trajectory.cpp
   )

set(HEADERS
        ModelPcm_Eval.h
        ResultDirItemModel.h
        TrajectoryTableModel.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Importer/SceneryImporterPCM.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_agent.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_course.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_data.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_globalData.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_line.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_lineContainer.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_marks.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_object.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_point.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_pointContainer.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/pcm/PCM_Data/pcm_trajectory.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME}_Models PRIVATE Qt5::Xml)
