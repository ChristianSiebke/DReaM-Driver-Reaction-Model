# /*********************************************************************
# * Copyright (c) 2020 ITK Engineering GmbH
# *
# * This program and the accompanying materials are made
# * available under the terms of the Eclipse Public License 2.0
# * which is available at https://www.eclipse.org/legal/epl-2.0/
# *
# * SPDX-License-Identifier: EPL-2.0
# **********************************************************************/

include_directories(../../../common)

set(SOURCES
        ModelStatistics.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/Histogram.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/RowHistograms.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/TableModel.cpp
   )

set(HEADERS
        ModelStatistics.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/Histogram.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/RowHistograms.h
        ${CMAKE_CURRENT_LIST_DIR}/../../../common/TableModel.h
    )

add_library(${PROJECT_NAME}_Models STATIC ${SOURCES} ${HEADERS})
